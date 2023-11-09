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

void SensorStateMachine::initializationRoutine(Data& dataContainer) {
    /**
     *  Am unsure of how exactly this is supposed to work. Some sensors (if not most) require a single initialization step,
     * in the form if a start_periodic_measurement() call. Measurements can then be read from the sensors in a given interval (example each 5k ms for SCD41)
     * See https://www.sensirion.com/media/documents/48C4B7FB/64C134E7/Sensirion_SCD4x_Datasheet.pdf
     *
     * Some sensors, like SGP41, require a initialization period over some period. This sensor requires a call to sgp41_execute_conditioning(), and can deliver readings not straight away but within a period of 10k ms max.
     * How do We handle this? The state machine has no way to plan ahead in the future, it can only make decisions based on previous decisions.
     *
     * In the case of SGP41, requesting a measurement with a frequency greater than 1/10 Hz should fail (if I understand the doc right)
     * See https://www.sensirion.com/media/documents/48C4B7FB/64C134E7/Sensirion_SCD4x_Datasheet.pdf
     */

    if (_initStepsCounter == 0) {
        for (size_t i = 0; i < getNumberOfDataPoints(); ++i) {
            dataContainer.addDataPoint(DataPoint(SignalType::UNDEFINED, 0.0, 0,
                                                 sensorName(getSensorId())));
        }
    }

    if (timeIntervalPassed(getInitializationIntervalMs(), millis(),
                           _lastMeasurementTimeStampMs)) {
        initializationStep();
        _initStepsCounter++;

        if (_initStepsCounter >= getInitializationSteps()) {
            _sensorState = SensorStatus::RUNNING;
        }
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
    // State handling
    switch (_sensorState) {
        case SensorStatus::UNDEFINED:
            break;

        case SensorStatus::INITIALIZING:
            initializationRoutine(data);
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
