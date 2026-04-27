#ifndef SEN69C_H
#define SEN69C_H

#include "ISensor.h"
#include "Sensirion_UPT_Core.h"
#include <SensirionI2cSen69c.h>

namespace sensirion::upt::i2c_autodetect{

inline const core::DeviceType SEN69C = core::DeviceTypeRegistry::CreateWiredDeviceType("SEN69C");


class Sen69c : public ISensor {
  public:
    explicit Sen69c(TwoWire& wire, uint16_t address);
    uint16_t start() override;
    uint16_t measureAndWrite(MeasurementList& measurements,
                             unsigned long timeStamp) override;
    uint16_t initializationStep() override;
    DeviceType getDeviceType() const override;
    core::MetaData getMetaData() const override;
    uint8_t getI2CAddress() const override;
    size_t getNumberOfDataPoints() const override;
    unsigned long getMinimumMeasurementIntervalMs() const override;
    bool probe() override;
    void* getDriver() override;

  private:
    TwoWire& mWire;
    SensirionI2cSen69c mDriver;
    uint16_t mAddress;
    core::MetaData mMetaData;
};
} // namespace sensirion::upt::i2c_autodetect

#endif /* SEN69C_H */
