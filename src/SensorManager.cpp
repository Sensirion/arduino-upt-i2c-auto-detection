#include "SensorManager.h"

static const char* TAG = "SensorManager";

void SensorManager::refreshConnectedSensors() {
    mSensorList.removeLostSensors();
    mDetector.findSensors(mSensorList);
}

void SensorManager::executeSensorCommunication() {
    for (int i = 0; i < mSensorList.count(); ++i) {
        SensorStateMachine* ssm = mSensorList.getSensorStateMachine(i);
        if (ssm) {
            const AutoDetectorError error = ssm->update();
            const char* sensorName =
                sensorLabel(ssm->getSensor()->getSensorType());
            switch (error) {
                case I2C_ERROR:
                    ESP_LOGW(TAG,
                             "An I2C error occurred while attempting to "
                             "execute a command on sensor %s.",
                             sensorName);
                    break;
                case LOST_SENSOR_ERROR:
                    ESP_LOGI(
                        TAG,
                        "Sensor %s was removed from list of active sensors.",
                        sensorName);
                    break;
                case SENSOR_READY_STATE_DECAYED_ERROR:
                    ESP_LOGW(
                        TAG,
                        "AutoDetect refresh rate too low: sensor %s "
                        "conditioning deprecated. Decrease update interval.",
                        sensorName);
                    break;
                case NO_ERROR:
                default:
                    break;
            }
        }
    }
}

void SensorManager::getSensorReadings(const MeasurementList** dataHashmap) {
    // Clear existing entries.
    // The order of the sensors in the measurement list depends on the availability
    // of sensors!
    memset(dataHashmap, 0, 
        sizeof(MeasurementList*)*mDetector.configuredSensorsCount());
    for (int i = 0; i < mSensorList.count(); ++i) {
        const SensorStateMachine* ssm = mSensorList.getSensorStateMachine(i);
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

void SensorManager::setInterval(const unsigned long interval,
                                const SensorType sensorType) {
    for (int i = 0; i < mSensorList.count(); ++i) {
        SensorStateMachine* ssm = mSensorList.getSensorStateMachine(i);
        if (ssm && ssm->getSensor()->getSensorType() == sensorType) {
            ssm->setMeasurementInterval(interval);
        }
    }
}

int SensorManager::getMaxNumberOfSensors() {
    return MAX_NUM_SENSORS;
}
