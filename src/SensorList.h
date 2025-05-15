#ifndef SENSOR_LIST_H
#define SENSOR_LIST_H
#include <vector>
#include "SensorStateMachine.h"


/* Class to handle the list of sensors on the i2c bus */
class SensorList {
  using SensorCollection = std::vector<SensorStateMachine*>;

  private:

    SensorCollection mSensorCollection{};
    static size_t hashSensorType(SensorType sensorType);

  public:
    explicit SensorList() {};

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
    void addSensor(ISensor* pSensor);

    /**
     * @brief Counts sensors contained in the list
     *
     * @returns number of sensors in the list
     */
    size_t count() const { return mSensorCollection.size(); };

    /**
     * @brief count Number of signals measured by all sensors contained in the
     * list
     *
     * @returns Number of DataPoints returned by the sensors in the
     * list
     */
    size_t getTotalNumberOfDataPoints() const;


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
    void removeLostSensors();
};

#endif /* SENSOR_LIST_H */
