#ifndef _SCD30_H_
#define _SCD30_H_

#include "ISensor.h"
#include "SensirionI2cScd30.h"
#include "Sensirion_UPT_Core.h"

class Scd30 : public ISensor {
  public:
    static const uint16_t I2C_ADDRESS = 0x61;
    explicit Scd30(TwoWire& wire) : _wire(wire){};
    uint16_t start() override;
    uint16_t measureAndWrite(Measurement measurements[],
                             const unsigned long timeStamp) override;
    uint16_t initializationStep() override;
    SensorType getSensorType() const override;
    size_t getNumberOfDataPoints() const override;
    unsigned long getMinimumMeasurementIntervalMs() const override;
    bool requiresInitializationStep() const override;
    void* getDriver() override;

  private:
    TwoWire& _wire;
    SensirionI2cScd30 _driver;
    const SensorType _sensorType = SensorType::SCD30;
    uint64_t _sensorID = 0;
};

#endif /* _SCD30_H_ */
