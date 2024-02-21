#include "SensorManager.h"

void SensorManager::refreshConnectedSensors() {
    _sensorList.removeLostSensors();
    _detector.findSensors(_sensorList);
}

void SensorManager::executeSensorCommunication() {
    for (int i = 0; i < _sensorList.getLength(); ++i) {
        SensorStateMachine* ssm = _sensorList.getSensorStateMachine(i);
        if (ssm) {
            AutoDetectorError error = ssm->update();
            const char* sensorName =
                sensorLabel(ssm->getSensor()->getSensorType());
            switch (error) {
                case I2C_ERROR:
                    Serial.printf("An I2C error occurred while attempting to "
                                  "execute a command on sensor %s.\n",
                                  sensorName);
                    break;
                case LOST_SENSOR_ERROR:
                    Serial.printf(
                        "Sensor %s was removed from list of active sensors.\n",
                        sensorName);
                    break;
                case SENSOR_READY_STATE_DECAYED_ERROR:
                    Serial.printf(
                        "AutoDetect refresh rate too low: sensor %s "
                        "conditioning deprecated. Decrease update interval.\n",
                        sensorName);
                    break;
                case NO_ERROR:
                default:
                    break;
            }
        }
    }
    return;
}

void SensorManager::getSensorReadings(const MeasurementList** dataHashmap) {
    for (int i = 0; i < _MAX_NUM_SENSORS; ++i) {
        const SensorStateMachine* ssm = _sensorList.getSensorStateMachine(i);
        if (ssm) {
            dataHashmap[i] = ssm->getSignals();
        }
    }
}

void SensorManager::refreshAndGetSensorReadings(
    const MeasurementList** dataHashmap) {
    refreshConnectedSensors();
    executeSensorCommunication();
    getSensorReadings(dataHashmap);
}

void SensorManager::setInterval(unsigned long interval, SensorType sensorId) {
    for (int i = 0; i < _sensorList.getLength(); ++i) {
        SensorStateMachine* ssm = _sensorList.getSensorStateMachine(i);
        if (ssm && ssm->getSensor()->getSensorType() == sensorId) {
            ssm->setMeasurementInterval(interval);
        }
    }
}

int SensorManager::getMaxNumberOfSensors() const {
    return _MAX_NUM_SENSORS;
}
