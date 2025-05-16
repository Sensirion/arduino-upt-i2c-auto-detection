#ifndef _SCD4X_H_
#define _SCD4X_H_

#include "ISensor.h"
#include "SensirionI2cScd4x.h"
#include "Sensirion_UPT_Core.h"
#include <string_view>

class Scd4x : public ISensor {
  public:
    explicit Scd4x(TwoWire& wire, uint16_t address);
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
    TwoWire& mWire;
    uint16_t mAddress;
    SensirionI2cScd4x mDriver;
    MetaData mMetadata;
};

#endif /* _SCD4X_H_ */
