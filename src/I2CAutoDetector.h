#ifndef _I2C_AUTO_DETECTOR_H_
#define _I2C_AUTO_DETECTOR_H_

#include "IAutoDetector.h"
#include <Wire.h>

class I2CAutoDetector : public IAutoDetector {
  public:
    explicit I2CAutoDetector(TwoWire& wire) : _wire(wire){};

    /**
     * @brief scan i2c bus for available sensors
     *
     * @param SensorList& SensorList to which add the found sensors
     */
    void findSensors(SensorList& sensorList) override;

  private:
    TwoWire& _wire;
    byte probeAddress(const byte& address);
    ISensor* createSensorFromAddress(const byte& address);
};

#endif /* _I2C_AUTO_DETECTOR_H_ */
