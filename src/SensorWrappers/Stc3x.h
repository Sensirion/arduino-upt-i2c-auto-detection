#ifndef _STC3X_H_
#define _STC3X_H_

#include "ISensor.h"
#include "Sensirion_UPT_Core.h"
#include <SensirionI2cStc3x.h>

namespace sensirion::upt::i2c_autodetect{

class Stc3x : public ISensor {
  public:
    explicit Stc3x(TwoWire& wire, uint16_t address);
    uint16_t start() override;
    uint16_t measureAndWrite(MeasurementList& measurements,
                             const unsigned long timeStamp) override;
    uint16_t initializationStep() override;
    DeviceType getDeviceType() const override;
    core::MetaData getMetaData() const override;
    size_t getNumberOfDataPoints() const override;
    unsigned long getMinimumMeasurementIntervalMs() const override;
    void* getDriver() override;

  private:
    TwoWire& _wire;
    uint16_t _address;
    SensirionI2cStc3x _driver;
    core::MetaData mMetadata;
};
} // namespace sensirion::upt::i2c_autodetect 

#endif /* _STC3X_H_ */
