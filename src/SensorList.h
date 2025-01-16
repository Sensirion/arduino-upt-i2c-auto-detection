#ifndef SENSOR_LIST_H
#define SENSOR_LIST_H

#include "SensorStateMachine.h"

/**
 * @note We reduce the size of the sensor universe for the purposes of
 * autodetection, since we cannot communicate with two sensors sharing the same
 * I2C-address. This allows us to use the address as an effective hash (using
 * SensorType would leave "holes" in the hashmap).
 */
enum class SensorHash {
    UNDEFINED,
    SCD4X,  // 0x62
    SFA3X,  // 0x5D
    SVM4X,  // 0x6A
    SHT4X,  // 0x44
    SEN5X,  // 0x69
    SCD30,  // 0x61
    STC3X,  // 0x29
    SGP4X,  // 0x59
    SEN66,  // 0x6b
};

/* Class to handle the list of sensors on the i2c bus */
class SensorList {
  private:
    const uint8_t mNumSensors;
    SensorStateMachine** mSensors = nullptr;
    static size_t hashSensorType(SensorType sensorType);

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
     * @param[in] pSensor pointer to the sensor to be added to the list
     */
    void addSensor(ISensor* pSensor) const;

    /**
     * @brief Counts sensors contained in the list
     *
     * @returns number of sensors in the list
     */
    size_t count() const;

    /**
     * @brief count Number of signals measured by all sensors contained in the
     * list
     *
     * @returns Number of DataPoints returned by the sensors in the
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
    SensorStateMachine* getSensorStateMachine(size_t) const;

    /**
     * @brief getter method for a stored sensor
     */
    ISensor* getSensor(SensorType sensorType) const;

    /**
     * @brief check if the given Sensor is contained in the list.
     *
     * @param[in] sensorType of the sensor to be checked for in the list
     *
     * @returns True if the sensor is found, false otherwise.
     */
    bool containsSensor(SensorType sensorType) const;

    /**
     * @brief remove lost sensors from list
     */
    void removeLostSensors() const;
};

#endif /* SENSOR_LIST_H */
