#ifndef I_AUTO_DETECTOR
#define I_AUTO_DETECTOR

#include "SensorList.h"

class IAutoDetector {
  public:
    virtual ~IAutoDetector() = default;
    /**
     * @brief Scans bus for Sensirion sensors, initializes them and
     * adds them to the passed sensor list.
     *
     * @note As the sensor list has a fixed length it might be that
     * not all detected sensors get registered. Adapt the LENGTH
     * in SensorList implementation in this case.
     *
     * @param sensorList list to add detected sensors
     */
    virtual void findSensors(SensorList& sensorList) = 0;
};

#endif /* I_AUTO_DETECTOR */
