#ifndef _SCD4X_H_
#define _SCD4X_H_

#include "ISensor.h"
#include "SensirionI2CScd4x.h"
#include "Sensirion_UPT_Core.h"

class Scd4x : public ISensor {
  public:
    static const uint16_t I2C_ADDRESS = 0x62;
    explicit Scd4x(TwoWire& wire);
    uint16_t start() override;
    uint16_t measureAndWrite(Measurement measurements[],
                             const unsigned long timeStamp) override;
    uint16_t initializationStep() override;
    SensorType getSensorType() const override;
    MetaData getMetaData() const override;
    size_t getNumberOfDataPoints() const override;
    unsigned long getMinimumMeasurementIntervalMs() const override;
    // Same as measurement interval
    unsigned long getInitializationIntervalMs() const override;
    void* getDriver() override;

  private:
    TwoWire& _wire;
    SensirionI2CScd4x _driver;
    MetaData _metaData;
};

#endif /* _SCD4X_H_ */
