#ifndef _SEN5X_H_
#define _SEN5X_H_

#include "ISensor.h"
#include "Sensirion_UPT_Core.h"
#include <SensirionI2CSen5x.h>

class Sen5x : public ISensor {
  public:
    explicit Sen5x(TwoWire& wire, uint16_t address);
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
    SensirionI2CSen5x _driver;
    uint16_t _address;
    MetaData _metaData;
    uint16_t _determineSensorVersion();
};

#endif /* _SEN5X_H_*/
