#ifndef _STCC4_H_
#define _STCC4_H_

#include "ISensor.h"
#include "Sensirion_UPT_Core.h"
#include <SensirionI2cStcc4.h>

class Stcc4 : public ISensor {
  public:
    explicit Stcc4(TwoWire& wire, uint16_t address);
    uint16_t start() override;
    uint16_t measureAndWrite(MeasurementList& measurements,
                             const unsigned long timeStamp) override;
    uint16_t initializationStep() override;
    DeviceType getDeviceType() const override;
    sensirion::upt::core::MetaData getMetaData() const override;
    size_t getNumberOfDataPoints() const override;
    unsigned long getMinimumMeasurementIntervalMs() const override;
    void* getDriver() override;

  private:
    TwoWire& _wire;
    uint16_t _address;
    SensirionI2cStcc4 _driver;
    sensirion::upt::core::MetaData mMetadata;
};

#endif /* _STCC4_H_ */
