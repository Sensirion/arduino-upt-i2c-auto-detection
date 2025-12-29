#ifndef _SCD30_H_
#define _SCD30_H_

#include "ISensor.h"
#include "SensirionI2cScd30.h"
#include "Sensirion_UPT_Core.h"

namespace sensirion::upt::i2c_autodetect{

class Scd30 : public ISensor {
  public:
    explicit Scd30(TwoWire& wire, uint16_t address);
    uint16_t start() override;
    uint16_t measureAndWrite(MeasurementList&,
                             const unsigned long timeStamp) override;
    uint16_t initializationStep() override;
    DeviceType getDeviceType() const override;
    core::MetaData getMetaData() const override;
    uint8_t getI2CAddress() const override;
    size_t getNumberOfDataPoints() const override;
    unsigned long getMinimumMeasurementIntervalMs() const override;
    void* getDriver() override;

  private:
    TwoWire& mWire;
    uint16_t mAddress;
    SensirionI2cScd30 mDriver;
    core::MetaData mMetadata;
};
} // namespace sensirion::upt::i2c_autodetect 

#endif /* _SCD30_H_ */
