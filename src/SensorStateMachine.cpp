/*
 * Copyright (c) 2023, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "SensorStateMachine.h"

bool timeIntervalPassed(const uint32_t interval,
                        const uint32_t currentTimeStamp,
                        const uint32_t latestUpdateTimeStamp) {
    if (currentTimeStamp < interval) {
        return true;
    }
    unsigned long elapsedTime = currentTimeStamp - latestUpdateTimeStamp;
    return elapsedTime >= interval;
}

SensorStateMachine::SensorStateMachine(ISensor* pSensor)
    : _sensorState(SensorStatus::UNINITIALIZED), _initErrorCounter(0),
      _measurementErrorCounter(0), _lastMeasurementTimeStampMs(0),
      _measurementIntervalMs(0), _sensor(pSensor) {
    _sensor->start();
};

AutoDetectorError SensorStateMachine::_initialize() {
    if (_sensor->requiresInitializationStep()) {
        uint16_t error = _sensor->initializationStep();
        if (error) {
            char errorMsg[256];
            errorToString(error, errorMsg, 256);
            Serial.printf("Failed to perform initialization step: %s\n",
                          errorMsg);
            return I2C_ERROR;
        }

        _lastMeasurementTimeStampMs = millis();
        _sensorState = SensorStatus::INITIALIZING;

        _sensorSignals.init(_sensor->getNumberOfDataPoints());

        for (size_t i = 0; i < _sensor->getNumberOfDataPoints(); ++i) {
            Measurement measurement;
            _sensorSignals.addMeasurement(measurement);
        }

    } else {
        _sensorState = SensorStatus::RUNNING;
        _sensorSignals.init(_sensor->getNumberOfDataPoints());
    }

    _measurementIntervalMs = _sensor->getMinimumMeasurementIntervalMs();

    return NO_ERROR;
}

void SensorStateMachine::_initializationRoutine() {
    if (timeIntervalPassed(_sensor->getInitializationIntervalMs(), millis(),
                           _lastMeasurementTimeStampMs)) {
        _sensorState = SensorStatus::RUNNING;
        _lastMeasurementTimeStampMs = millis();
    }
}

AutoDetectorError SensorStateMachine::_readSignalsRoutine() {
    unsigned long timeSinceLastMeasurementMs =
        millis() - _lastMeasurementTimeStampMs;

    /* Determine timing relationship vs. last measurement */
    enum class timeLineRegion {
        INSIDE_MIN_INTERVAL,          // Measurement not yet due to be performed
        INSIDE_VALID_BAND,            // May request a reading
        OUTSIDE_VALID_INITIALIZATION  // Sensor running status has decayed,
                                      // conditionning must be performed
    };
    timeLineRegion tlr_position = timeLineRegion::INSIDE_MIN_INTERVAL;

    if (timeSinceLastMeasurementMs >= _measurementIntervalMs) {
        tlr_position = timeLineRegion::INSIDE_VALID_BAND;
    }

    if (_sensor->readyStateDecayTimeMs() > 0 &&
        timeSinceLastMeasurementMs > _sensor->readyStateDecayTimeMs()) {
        tlr_position = timeLineRegion::OUTSIDE_VALID_INITIALIZATION;
    }

    /* Perform appropriate action */
    AutoDetectorError error = NO_ERROR;
    switch (tlr_position) {
        case timeLineRegion::INSIDE_MIN_INTERVAL:
            break;

        case timeLineRegion::INSIDE_VALID_BAND:
            error = _readSignals();
            if (error) {
                return error;
            }
            break;

        case timeLineRegion::OUTSIDE_VALID_INITIALIZATION:
            _sensorState = SensorStatus::UNINITIALIZED;
            return SENSOR_READY_STATE_DECAYED_ERROR;

        default:
            break;
    }

    return NO_ERROR;
}

AutoDetectorError SensorStateMachine::_readSignals() {
    Measurement signalsBuf[_sensor->getNumberOfDataPoints()];
    uint32_t nowMS = millis();

    uint16_t error = _sensor->measureAndWrite(signalsBuf, nowMS);

    if (error) {
        char errorMsg[256];
        errorToString(error, errorMsg, 256);
        Serial.printf("Failed to read measurements for sensor %s: %s\n",
                      sensorLabel(_sensor->getSensorType()), errorMsg);
        return I2C_ERROR;
    }

    _lastMeasurementTimeStampMs = nowMS;

    for (size_t i = 0; i < _sensor->getNumberOfDataPoints(); ++i) {
        _sensorSignals.addMeasurement(signalsBuf[i]);
    }
    _sensorSignals.resetWriteHead();

    return NO_ERROR;
}

SensorStatus SensorStateMachine::getSensorState() const {
    return _sensorState;
}

uint16_t SensorStateMachine::setMeasurementInterval(uint32_t interval) {
    if (interval > _sensor->getMinimumMeasurementIntervalMs()) {
        _measurementIntervalMs = interval;
        return NO_ERROR;
    }
    return 1;
}

AutoDetectorError SensorStateMachine::update() {
    AutoDetectorError error = NO_ERROR;
    switch (_sensorState) {
        case SensorStatus::UNDEFINED:
            break;

        case SensorStatus::UNINITIALIZED:
            error = _initialize();
            if (error) {
                _initErrorCounter++;
            } else {
                _initErrorCounter = 0;
            }
            break;

        case SensorStatus::INITIALIZING:
            _initializationRoutine();
            break;

        case SensorStatus::RUNNING:
            error = _readSignalsRoutine();
            if (error) {
                _measurementErrorCounter++;
            } else {
                _measurementErrorCounter = 0;
            }
            break;

        case SensorStatus::LOST:
            break;

        default:
            break;
    }

    uint16_t nAllowed = _sensor->getNumberOfAllowedConsecutiveErrors();
    if (_initErrorCounter > nAllowed || _measurementErrorCounter > nAllowed) {
        _sensorState = SensorStatus::LOST;
        return LOST_SENSOR_ERROR;
    }

    return error;
}

ISensor* SensorStateMachine::getSensor() const {
    return _sensor;
}

const MeasurementList* SensorStateMachine::getSignals() const {
    return &_sensorSignals;
}
