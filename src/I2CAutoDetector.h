#ifndef I2C_AUTO_DETECTOR_H
#define I2C_AUTO_DETECTOR_H

#include "IAutoDetector.h"
#include <Wire.h>

class I2CAutoDetector : public IAutoDetector {
  public:
    explicit I2CAutoDetector(TwoWire& wire) : _wire(wire){};

    /**
     * @brief scan i2c bus for available sensors
     *
     * @param sensorList SensorList to which add the found sensors
     */
    void findSensors(SensorList& sensorList) override;

  private:
    TwoWire& _wire;
    SensorType probeAddress(const byte& address) const;
    ISensor* createSensorFromSensorType(SensorType sensorType) const;
    static SensorType getSensorTypeFromAddress(byte address);
};

#endif /* I2C_AUTO_DETECTOR_H */
