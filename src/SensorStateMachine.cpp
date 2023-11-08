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

void SensorStateMachine::readSignalsRoutine(Data& dataContainer) {
    if (timeIntervalPassed(getMeasurementInterval(), millis(),
                           _lastMeasurementTimeStampMs)) {
        DataPoint sensorSignalsBuffer[getNumberOfDataPoints()];
        uint32_t currentTimeStampMs = millis();

        uint16_t measureAndWriteError =
            measureAndWrite(sensorSignalsBuffer, currentTimeStampMs);
        _lastMeasurementError = measureAndWriteError;

        if (measureAndWriteError) {
            _measurementErrorCounter++;
            if (_measurementErrorCounter >=
                getNumberOfAllowedConsecutiveErrors()) {
                _sensorState = SensorStatus::LOST;
            }
            return;
        }

        _lastMeasurementTimeStampMs = currentTimeStampMs;
        _measurementErrorCounter = 0;

        for (size_t i = 0; i < getNumberOfDataPoints(); ++i) {
            dataContainer.addDataPoint(sensorSignalsBuffer[i]);
        }
    }
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
            readSignalsRoutine(data);
            break;

        case SensorStatus::LOST:
            break;

        default:
            break;
    }

    return;
}
