#ifndef _SHT4X_H_
#define _SHT4X_H_

#include "ISensor.h"
#include "SensirionI2CSht4x.h"
#include "Sensirion_UPT_Core.h"

class Sht4x : public ISensor {
  public:
    static const uint16_t I2C_ADDRESS = 0x44;
    explicit Sht4x(TwoWire& wire) : _wire(wire){};
    uint16_t start() override;
    uint16_t measureAndWrite(Measurement measurements[],
                             const unsigned long timeStamp) override;
    SensorType getSensorType() const override;
    size_t getNumberOfDataPoints() const override;
    unsigned long getMinimumMeasurementIntervalMs() const override;
    void* getDriver() override;

  private:
    TwoWire& _wire;
    SensirionI2CSht4x _driver;
    const SensorType _sensorType = SensorType::SHT4X;
    uint64_t _sensorID = 0;
};

#endif /* _SHT4X_H_ */
