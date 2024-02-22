#ifndef _I_AUTO_DETECTOR_
#define _I_AUTO_DETECTOR_

#include "SensorList.h"

class IAutoDetector {
  public:
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

#endif /* _I_AUTO_DETECTOR_ */
