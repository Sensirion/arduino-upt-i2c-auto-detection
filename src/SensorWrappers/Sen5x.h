#ifndef _SEN5X_H_
#define _SEN5X_H_

#include "ISensor.h"
#include "Sensirion_UPT_Core.h"
#include <SensirionI2CSen5x.h>

class Sen5x : public ISensor {
  public:
    static const uint16_t I2C_ADDRESS = 0x69;
    explicit Sen5x(TwoWire& wire) : _wire(wire){};
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
    SensirionI2CSen5x _driver;
    SensorType _sensorType =
        SensorType::SEN5X;  // determined more precisely in initializationStep()
    uint64_t _sensorID = 0;
    uint16_t _determineSensorVersion();
};

#endif /* _SEN5X_H_*/
