#ifndef _SVM40_H_
#define _SVM40_H_

#include "ISensor.h"
#include "SensirionI2CSvm41.h"
#include "Sensirion_UPT_Core.h"

namespace sensirion::upt::i2c_autodetect{

class Svm4x : public ISensor {
  public:
    explicit Svm4x(TwoWire& wire, uint16_t address);
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
    SensirionI2CSvm41 mDriver;
    core::MetaData mMetaData;
};
} // namespace sensirion::upt::i2c_autodetect 

#endif /* _SVM40_H_ */
