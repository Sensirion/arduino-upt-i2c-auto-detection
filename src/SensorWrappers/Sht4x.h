#ifndef _SHT4X_H_
#define _SHT4X_H_

#include "ISensor.h"
#include "SensirionI2cSht4x.h"
#include "Sensirion_UPT_Core.h"

class Sht4x : public ISensor {
  public:
    explicit Sht4x(TwoWire& wire, uint16_t address);
    uint16_t start() override;
    uint16_t measureAndWrite(MeasurementList& measurements,
                             const unsigned long timeStamp) override;
    uint16_t initializationStep() override;
    DeviceType getSensorType() const override;
    upt_core::MetaData getMetaData() const override;
    size_t getNumberOfDataPoints() const override;
    unsigned long getMinimumMeasurementIntervalMs() const override;
    void* getDriver() override;

  private:
    TwoWire& _wire;
    uint16_t _address;
    SensirionI2cSht4x _driver;
    upt_core::MetaData mMetadata;
};

#endif /* _SHT4X_H_ */
