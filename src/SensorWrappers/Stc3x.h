#ifndef _STC3X_H_
#define _STC3X_H_

#include "ISensor.h"
#include "Sensirion_UPT_Core.h"
#include <SensirionI2cStc3x.h>

class Stc3x : public ISensor {
  public:
    explicit Stc3x(TwoWire& wire, uint16_t address);
    uint16_t start() override;
    uint16_t measureAndWrite(Measurement measurements[],
                             const unsigned long timeStamp) override;
    uint16_t initializationStep() override;
    SensorType getSensorType() const override;
    MetaData getMetaData() const override;
    size_t getNumberOfDataPoints() const override;
    unsigned long getMinimumMeasurementIntervalMs() const override;
    void* getDriver() override;

  private:
    TwoWire& _wire;
    uint16_t _address;
    SensirionI2cStc3x _driver;
    MetaData _metaData;
};

#endif /* _STC3X_H_ */
