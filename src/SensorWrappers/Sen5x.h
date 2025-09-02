#ifndef _SEN5X_H_
#define _SEN5X_H_

#include "ISensor.h"
#include "Sensirion_UPT_Core.h"
#include <SensirionI2CSen5x.h>

namespace sensirion::upt::i2c_autodetect{

class Sen5x : public ISensor {
  public:
    explicit Sen5x(TwoWire& wire, uint16_t address);
    uint16_t start() override;
    uint16_t measureAndWrite(MeasurementList&,
                             const unsigned long timeStamp) override;
    uint16_t initializationStep() override;
    DeviceType getDeviceType() const override;
    sensirion::upt::core::MetaData getMetaData() const override;
    size_t getNumberOfDataPoints() const override;
    unsigned long getMinimumMeasurementIntervalMs() const override;
    void* getDriver() override;

  private:
    TwoWire& _wire;
    SensirionI2CSen5x _driver;
    uint16_t _address;
    sensirion::upt::core::MetaData _metaData;
    uint16_t _determineSensorVersion();
};
} // namespace sensirion::upt::i2c_autodetect 

#endif /* _SEN5X_H_*/
