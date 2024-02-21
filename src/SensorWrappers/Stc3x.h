#ifndef _STC3X_H_
#define _STC3X_H_

#include "ISensor.h"
#include "Sensirion_UPT_Core.h"
#include <SensirionI2CStc3x.h>

class Stc3x : public ISensor {
  public:
    static const uint16_t I2C_ADDRESS = 0x29;
    explicit Stc3x(TwoWire& wire) : _wire(wire){};
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
    SensirionI2CStc3x _driver;
    SensorType _sensorType =
        SensorType::STC3X;  // Determined more precisely at initializationStep()
    uint64_t _sensorID = 0;
};

#endif /* _STC3X_H_ */
