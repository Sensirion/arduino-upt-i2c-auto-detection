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
    : _sensorState(SensorStatus::UNINITIALIZED), _lastMeasurementError(0),
      _measurementErrorCounter(0), _lastMeasurementTimeStampMs(0),
      _measurementIntervalMs(0), _sensor(pSensor) {
    _sensor->start();
};

void SensorStateMachine::_initialize() {
    if (_sensor->requiresInitializationStep()) {
        _sensor->initializationStep();
        _lastMeasurementTimeStampMs = millis();
        _sensorState = SensorStatus::INITIALIZING;

        _measurementIntervalMs = _sensor->getMinimumMeasurementIntervalMs();
        _sensorSignals.init(_sensor->getNumberOfDataPoints());

        for (size_t i = 0; i < _sensor->getNumberOfDataPoints(); ++i) {
            _sensorSignals.addDataPoint(
                DataPoint(SignalType::UNDEFINED, 0.0, 0,
                          sensorName(_sensor->getSensorType())));
        }

    } else {
        _sensorState = SensorStatus::RUNNING;
        _measurementIntervalMs = _sensor->getMinimumMeasurementIntervalMs();
        _sensorSignals.init(_sensor->getNumberOfDataPoints());
    }
}

void SensorStateMachine::_initializationRoutine() {
    if (timeIntervalPassed(_sensor->getInitializationIntervalMs(), millis(),
                           _lastMeasurementTimeStampMs)) {
        _sensorState = SensorStatus::RUNNING;
        _lastMeasurementTimeStampMs = millis();
    }
}

void SensorStateMachine::_readSignalsRoutine() {
    unsigned long timeSinceLastMeasurementMs =
        millis() - _lastMeasurementTimeStampMs;

    /* Determine timing relationship vs. last measurement */
    enum class timeLineRegion {
        INSIDE_MIN_INTERVAL,  // Less time than measuring interval has elapsed
                              // since last measurement
        INSIDE_VALID_BAND,    // May request a reading
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
    switch (tlr_position) {
        case timeLineRegion::INSIDE_MIN_INTERVAL:
            break;

        case timeLineRegion::INSIDE_VALID_BAND:
            _readSignals();
            break;

        case timeLineRegion::OUTSIDE_VALID_INITIALIZATION:
            _sensorState = SensorStatus::UNINITIALIZED;
            // throw error
            break;

        default:
            break;
    }
}

void SensorStateMachine::_readSignals() {
    DataPoint sensorSignalsBuffer[_sensor->getNumberOfDataPoints()];
    uint32_t currentTimeStampMs = millis();

    uint16_t measureAndWriteError =
        _sensor->measureAndWrite(sensorSignalsBuffer, currentTimeStampMs);
    _lastMeasurementError = measureAndWriteError;

    if (measureAndWriteError) {
        _measurementErrorCounter++;
        if (_measurementErrorCounter >=
            _sensor->getNumberOfAllowedConsecutiveErrors()) {
            _sensorState = SensorStatus::LOST;
        }
        return;
    }

    _lastMeasurementTimeStampMs = currentTimeStampMs;
    _measurementErrorCounter = 0;

    for (size_t i = 0; i < _sensor->getNumberOfDataPoints(); ++i) {
        _sensorSignals.addDataPoint(sensorSignalsBuffer[i]);
    }
    _sensorSignals.resetWriteHead();
}

SensorStatus SensorStateMachine::getSensorState() const {
    return _sensorState;
}

void SensorStateMachine::setSensorState(SensorStatus s) {
    _sensorState = s;
}

void SensorStateMachine::setMeasurementInterval(uint32_t interval) {
    if (interval > _sensor->getMinimumMeasurementIntervalMs()) {
        _measurementIntervalMs = interval;
    }
}

void SensorStateMachine::update() {
    switch (_sensorState) {
        case SensorStatus::UNDEFINED:
            break;

        case SensorStatus::UNINITIALIZED:
            _initialize();
            break;

        case SensorStatus::INITIALIZING:
            _initializationRoutine();
            break;

        case SensorStatus::RUNNING:
            _readSignalsRoutine();
            break;

        case SensorStatus::LOST:
            break;

        default:
            break;
    }
    return;
}

ISensor* SensorStateMachine::getSensor() const {
    return _sensor;
}

const DataPointList* SensorStateMachine::getSignals() const {
    return &_sensorSignals;
}
