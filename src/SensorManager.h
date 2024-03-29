#ifndef _SENSOR_MANAGER_H_
#define _SENSOR_MANAGER_H_

#include "IAutoDetector.h"
#include "MeasurementList.h"
#include "SensirionCore.h"
#include "SensorList.h"

/* Class to manage the sensors connected to the board's I2C bus. Handles
 * detection and signal polling in accordance to the sensor's minimal and
 * maximal polling intervals */
class SensorManager {
  private:
    // Number of sensors is limited by size of hash in _sensorList.
    static const uint8_t _MAX_NUM_SENSORS = 8;
    SensorList _sensorList;
    IAutoDetector& _detector;

  public:
    /**
     * @brief constructor
     *
     * @param[in] IAutoDetector& detector instance with which to seek for
     * connected sensors
     *
     * @note With this construction we by default limit ourselves to one I2C
     * bus, ie. it is not possible to seek for sensors on both the 3.3V and 5V
     * buses. Users may start another instance of Sensormanager to handle such
     * use cases.
     */
    explicit SensorManager(IAutoDetector& detector_)
        : _sensorList(_MAX_NUM_SENSORS), _detector(detector_){};

    /**
     * @brief Remove lost sensors and check bus for connected sensors
     */
    void refreshConnectedSensors();

    /**
     * @brief Updates all sensor state machines, which fetches signal updates
     * (whenever available)
     */
    void executeSensorCommunication();

    /**
     * @brief obtain a hashmap of read-only pointers to the sensor signal
     * readings.
     *
     * @param[in] DataPointList** location to which write the references to the
     * individual state machines data, hashed by their respective SensorIDs.
     * Size of the hashmap can be queried using
     * SensorManager::getMaxNumberOfSensors(). Existing entries are either
     * ignored or overwritten.
     */
    void getSensorReadings(const MeasurementList**);

    /**
     * @brief convenience function performing the sensor list refresh, state
     * machine update and data window setup
     *
     * @param[in] MeasurementList** location to which write the references to
     * the individual state machines data, hashed by their respective
     * SensorTypes. Size of the hashmap can be queried using
     * SensorManager::getMaxNumberOfSensors(). Existing entries are either
     * ignored or overwritten.
     */
    void refreshAndGetSensorReadings(const MeasurementList**);

    /**
     * @brief Sets polling interval for the specified sensor after checking if
     * it is valid
     *
     * @param[in] interval desired measurement interval
     *
     * @param[in] SensorType target sensor
     *
     * @note Does not return an error in case the validity checks fail, in which
     * case the interval is not set for the sensor
     */
    void setInterval(unsigned long interval, SensorType sensorId);

    /**
     * @brief getter method for number of sensors
     */
    int getMaxNumberOfSensors() const;

    /**
     * Retrieve specific sensor driver instance T from
     * SensorManager::_sensorList
     *
     * @param[in] pDriver nullptr initialized pointer to specific Sensirion
     * sensor driver class T.
     *
     * @param[in] SensorType corresponding to ISensor implementation for sensor
     * driver class T.
     *
     * @param[out] AutodDetectorError::DRIVER_NOT_FOUND_ERROR in case of failure
     * to retrieve the driver. Only in case of NO_ERROR may the driver methods
     * be called. e.g.: pDriver->driverMethod()
     */
    template <class T>
    AutoDetectorError getSensorDriver(T*& pDriver, SensorType sensorType) {
        if (!_sensorList.containsSensor(sensorType)) {
            return DRIVER_NOT_FOUND_ERROR;
        } else {
            pDriver = reinterpret_cast<T*>(
                _sensorList.getSensor(sensorType)->getDriver());
        }
        return NO_ERROR;
    };
};

#endif /* _SENSOR_MANAGER_H_ */
