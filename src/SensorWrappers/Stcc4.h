#ifndef _STCC4_H_
#define _STCC4_H_

#include "ISensor.h"
#include "Sensirion_UPT_Core.h"
#include <SensirionI2cStcc4.h>

namespace sensirion::upt::i2c_autodetect{

class Stcc4 : public ISensor {
  public:
    explicit Stcc4(TwoWire& wire, uint16_t address);
    uint16_t start() override;
    uint16_t measureAndWrite(MeasurementList& measurements,
                             const unsigned long timeStamp) override;
    uint16_t initializationStep() override;
    DeviceType getDeviceType() const override;
    core::MetaData getMetaData() const override;
    size_t getNumberOfDataPoints() const override;
    uint8_t getI2CAddress() const override;
    unsigned long getMinimumMeasurementIntervalMs() const override;
    void* getDriver() override;

  private:
    TwoWire& mWire;
    uint16_t mAddress;
    SensirionI2cStcc4 mDriver;
    core::MetaData mMetadata;
};
} // namespace sensirion::upt::i2c_autodetect 

#endif /* _STCC4_H_ */
