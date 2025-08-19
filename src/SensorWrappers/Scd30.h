#ifndef _SCD30_H_
#define _SCD30_H_

#include "ISensor.h"
#include "SensirionI2cScd30.h"
#include "Sensirion_UPT_Core.h"

class Scd30 : public ISensor {
  public:
    explicit Scd30(TwoWire& wire, uint16_t address);
    uint16_t start() override;
    uint16_t measureAndWrite(MeasurementList&,
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
    SensirionI2cScd30 _driver;
    upt_core::MetaData _metaData;
};

#endif /* _SCD30_H_ */
