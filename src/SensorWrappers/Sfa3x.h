#ifndef _SFA3X_H_
#define _SFA3X_H_

#include "ISensor.h"
#include "Sensirion_UPT_Core.h"
#include <SensirionI2cSfa3x.h>

class Sfa3x : public ISensor {
  public:
    static const uint16_t I2C_ADDRESS = SFA3X_I2C_ADDR_5D;
    explicit Sfa3x(TwoWire& wire);
    uint16_t start() override;
    uint16_t measureAndWrite(Measurement measurements[],
                             const unsigned long timeStamp) override;
    uint16_t initializationStep() override;
    SensorType getSensorType() const override;
    MetaData getMetaData() const override;
    size_t getNumberOfDataPoints() const override;
    unsigned long getMinimumMeasurementIntervalMs() const override;
    void* getDriver() override;

  private:
    TwoWire& _wire;
    SensirionI2cSfa3x _driver;
    MetaData _metaData;
};

#endif /* _SFA3X_H_ */
