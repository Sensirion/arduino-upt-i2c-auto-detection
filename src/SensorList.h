#ifndef _SENSOR_LIST_H
#define _SENSOR_LIST_H

#include "AutoDetectorErrors.h"
#include "SensorStateMachine.h"

/**
 * @note We reduce the size of the sensor universe for the purposes of
 * autodetection, since we cannot communicate with two sensors sharing the same
 * I2C-address. This allows us to use the address as an effective hash (using
 * SensorType would leave "holes" in the hashmap).
 */
enum class SensorHash {
    _UNDEFINED,
    _SCD4X,  // 0x62
    _SFA3X,  // 0x5D
    _SVM4X,  // 0x6A
    _SHT4X,  // 0x44
    _SEN5X,  // 0x69
    _SCD30,  // 0x61
    _STC3X,  // 0x29
    _SGP4X,  // 0x59
};

/* Class to handle the list of sensors on the i2c bus */
class SensorList {
  private:
    const uint8_t _numSensors;
    SensorStateMachine** _sensors = nullptr;
    size_t _hashSensorType(SensorType sensorType) const;

  public:
    explicit SensorList(uint8_t numSensors);

    SensorList(const SensorList&) = delete;  // Illegal operation
    SensorList&
    operator=(const SensorList& other) = delete;  // Illegal operation

    ~SensorList();

    /**
     * @brief add a sensor to the list of tracked sensors. Ignores sensors that
     * are already in the list. Causes program abort if unknown sensor is
     * encountered.
     *
     * @param[in] ISensor* pointer to the sensor to be added to the list
     */
    void addSensor(ISensor* pSensor);

    /**
     * @brief Counts sensors contained in the list
     *
     * @param[out] size_t number of sensors in the list
     */
    size_t count();

    /**
     * @brief count number of signals measured by all sensors contained in the
     * list
     *
     * @param[out] size_t number of DataPoints returned by the sensors in the
     * list
     */
    size_t getTotalNumberOfDataPoints() const;

    /**
     * @brief getter method for list size
     */
    int getLength() const;

    /**
     * @brief getter method for a stored state machine
     *
     * @note returns a nullptr if no sensor state machine is stored at the
     * requested index
     */
    SensorStateMachine* getSensorStateMachine(size_t);

    /**
     * @brief getter method for a stored sensor
     */
    ISensor* getSensor(SensorType sensorType) const;

    /**
     * @brief check if the given Sensor is contained in the list.
     *
     * @param[in] SensorType of the sensor to be checked for in the list
     *
     * @param[out] bool: True if the sensor is found, false otherwise.
     */
    bool containsSensor(SensorType) const;

    /**
     * @brief remove lost sensors from list
     */
    void removeLostSensors();
};

#endif /* _SENSOR_LIST_H */
