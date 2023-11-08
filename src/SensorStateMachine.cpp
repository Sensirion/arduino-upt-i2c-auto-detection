#include "SensorStateMachine.h"
#include "utils.h"

SensorStatus SensorStateMachine::getSensorState() const {
    return _sensorState;
}

void SensorStateMachine::setSensorState(SensorStatus s) {
    _sensorState = s;
}

uint32_t SensorStateMachine::getMeasurementInterval() const {
    if (_customMeasurementIntervalMs > getMinimumMeasurementIntervalMs()) {
        return _customMeasurementIntervalMs;
    } else {
        return getMinimumMeasurementIntervalMs();
    }
}

void SensorStateMachine::setMeasurementInterval(uint32_t interval) {
    if (interval > getMinimumMeasurementIntervalMs()) {
        _customMeasurementIntervalMs = interval;
    }
}

void SensorStateMachine::readSignals(DataPoint* signalsBuffer,
                                     uint32_t timeStampMs) {
    uint16_t measureAndWriteError = measureAndWrite(signalsBuffer, timeStampMs);
    _lastMeasurementError = measureAndWriteError;

    if (measureAndWriteError) {
        _measurementErrorCounter++;
        if (_measurementErrorCounter >= getNumberOfAllowedConsecutiveErrors()) {
            _sensorState = SensorStatus::LOST;
        }
    }

    _lastMeasurementTimeStampMs = timeStampMs;
    _measurementErrorCounter = 0;
}

void SensorStateMachine::updateSensorSignals(Data& data) {
    // Collect variables for readability
    unsigned long currentTimeStampMs = millis();
    unsigned long initSteps = getInitializationSteps();
    unsigned long initIntervalMs = getInitializationIntervalMs();
    unsigned long measureIntervalMs = getMeasurementInterval();

    uint16_t measureAndWriteError = 0x1234;
    DataPoint sensorSignalsBuffer[getNumberOfDataPoints()];

    // State handling
    switch (_sensorState) {
        case SensorStatus::UNDEFINED:
            break;

        case SensorStatus::INITIALIZING:
            // Set Sensor name of empty Datapoints for initialization period
            if (_initStepsCounter == 0) {
                for (size_t i = 0; i < getNumberOfDataPoints(); ++i) {
                    data.addDataPoint(DataPoint(SignalType::UNDEFINED, 0.0, 0,
                                                sensorName(getSensorId())));
                }
            }

            // Only perform initialization every initialization interval
            if (!timeIntervalPassed(initIntervalMs, currentTimeStampMs,
                                    _lastMeasurementTimeStampMs)) {
                break;
            }
            initializationStep();
            _initStepsCounter++;

            // Check if initialization is done
            if (_initStepsCounter >= initSteps) {
                setSensorState(SensorStatus::RUNNING);
            }
            break;

        case SensorStatus::RUNNING:
            // Only perform measurement every measurement interval
            if (!timeIntervalPassed(measureIntervalMs, currentTimeStampMs,
                                    _lastMeasurementTimeStampMs)) {
                break;
            }

            // Perform measurement
            readSignals(sensorSignalsBuffer, currentTimeStampMs);

            // Push DataPoints to Data container
            for (size_t i = 0; i < getNumberOfDataPoints(); ++i) {
                data.addDataPoint(sensorSignalsBuffer[i]);
            }

            break;

        case SensorStatus::LOST:
            break;

        default:
            break;
    }

    return;
}
