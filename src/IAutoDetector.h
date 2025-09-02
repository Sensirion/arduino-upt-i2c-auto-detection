#ifndef I_AUTO_DETECTOR
#define I_AUTO_DETECTOR

#include "SensorList.h"

namespace sensirion::upt::i2c_autodetect{

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

    /**
     * Get the number of configured sensors
     *
     * @return The number of sensors that can be configured
    */
    virtual size_t configuredSensorsCount() const = 0;
};
} // namespace sensirion::upt::i2c_autodetect 

#endif /* I_AUTO_DETECTOR */
