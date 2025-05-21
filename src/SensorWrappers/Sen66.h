#ifndef SEN66_H
#define SEN66_H_

#include "ISensor.h"
#include "Sensirion_UPT_Core.h"
#include <SensirionI2cSen66.h>

class Sen66 : public ISensor {
  public:
    explicit Sen66(TwoWire& wire, uint16_t address);
    uint16_t start() override;
    uint16_t measureAndWrite(Measurement measurements[],
                             unsigned long timeStamp) override;
    uint16_t initializationStep() override;
    SensorType getSensorType() const override;
    MetaData getMetaData() const override;
    size_t getNumberOfDataPoints() const override;
    unsigned long getMinimumMeasurementIntervalMs() const override;
    unsigned long getInitializationIntervalMs() const override;
    void* getDriver() override;

  private:
    TwoWire& mWire;
    SensirionI2cSen66 mDriver;
    uint16_t mAddress;
    MetaData mMetaData;
};

#endif /* SEN66_H*/
