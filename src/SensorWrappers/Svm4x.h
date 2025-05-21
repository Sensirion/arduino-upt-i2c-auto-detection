#ifndef _SVM40_H_
#define _SVM40_H_

#include "ISensor.h"
#include "SensirionI2CSvm41.h"
#include "Sensirion_UPT_Core.h"

class Svm4x : public ISensor {
  public:
    explicit Svm4x(TwoWire& wire, uint16_t address);
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
    SensirionI2CSvm41 _driver;
    MetaData _metaData;
};

#endif /* _SVM40_H_ */
