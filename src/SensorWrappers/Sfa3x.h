#ifndef _SFA3X_H_
#define _SFA3X_H_

#include "ISensor.h"
#include "Sensirion_UPT_Core.h"
#include <SensirionI2CSfa3x.h>

class Sfa3x : public ISensor {
  public:
    static const uint16_t I2C_ADDRESS = 0x5D;
    explicit Sfa3x(TwoWire& wire) : _wire(wire){};
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
    SensirionI2CSfa3x _driver;
    const SensorType _sensorType = SensorType::SFA3X;
    uint64_t _sensorID = 0;
};

#endif /* _SFA3X_H_ */
