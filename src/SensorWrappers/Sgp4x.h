#ifndef _SGP4X_H_
#define _SGP4X_H_

#include "ISensor.h"
#include "SensirionI2CSgp41.h"
#include "Sensirion_UPT_Core.h"

class Sgp41 : public ISensor {
  public:
    explicit Sgp41(TwoWire& wire, uint16_t address);
    uint16_t start() override;
    uint16_t measureAndWrite(MeasurementList& measurements,
                             const unsigned long timeStamp) override;
    uint16_t initializationStep() override;
    DeviceType getSensorType() const override;
    sensirion::upt::core::MetaData getMetaData() const override;
    size_t getNumberOfDataPoints() const override;
    unsigned long getMinimumMeasurementIntervalMs() const override;

    // Typical: 10s
    unsigned long getInitializationIntervalMs() const override;
    void* getDriver() override;

    // Typical measurement interval: 1s
    long readyStateDecayTimeMs() const override;

  private:
    TwoWire& _wire;
    SensirionI2CSgp41 _driver;
    uint16_t _address;
    sensirion::upt::core::MetaData mMetadata;
    uint16_t _defaultRh = 0x8000;
    uint16_t _defaultT = 0x6666;
};

#endif /* _SGP4X_H_ */
