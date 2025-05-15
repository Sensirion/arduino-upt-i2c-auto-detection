#ifndef I2C_AUTO_DETECTOR_H
#define I2C_AUTO_DETECTOR_H

#include "IAutoDetector.h"
#include <Wire.h>
#include <array>
#include <initializer_list>
#include <vector>

struct ISensorToAddressMapping{
  using Iterator = ISensor*;
  using ConstIterator = const ISensor*;

  virtual uint8_t getI2cAddress() const = 0;
  virtual size_t size() const = 0;
  virtual Iterator at(size_t index) const = 0;
  virtual ~ISensorToAddressMapping(){}
};

template<uint8_t address, typename...SensorT>
struct SensorToAddressMapping: ISensorToAddressMapping{

  using SensorOnAddress = std::array<ISensor*, sizeof...(SensorT)>;
  using Iterator = typename ISensorToAddressMapping::Iterator;
  using ConstIterator = typename ISensorToAddressMapping::ConstIterator;

  static constexpr uint8_t I2C_ADDRESS = address;

  SensorToAddressMapping(TwoWire& wire):
    mOnAddressSensors{(createSensorInstance<SensorT>(wire), ...)} {}

  virtual ~SensorToAddressMapping() override {
    for(auto s : mOnAddressSensors){
      delete s;
    }
  }

  SensorToAddressMapping(const SensorToAddressMapping& other) = delete;
  SensorToAddressMapping(const SensorToAddressMapping&& other) = delete;
  SensorToAddressMapping& operator=(const SensorToAddressMapping& other) = delete;

  Iterator begin(){
    return mOnAddressSensors.begin();
  }

  Iterator end(){
    return mOnAddressSensors.end();
  }

  ConstIterator cbegin() const {
    return mOnAddressSensors.cbegin();
  }

  ConstIterator cend() const{
    return mOnAddressSensors.cend();
  }

  virtual  size_t size()  const override{
    return mOnAddressSensors.size();
  }

  virtual  Iterator at(size_t index) const override {
    return mOnAddressSensors.at(index);
  }

  virtual uint8_t getI2cAddress()  const override{
    return I2C_ADDRESS;
  }

  private:
    SensorOnAddress mOnAddressSensors;

    template<typename T>
    ISensor* createSensorInstance(TwoWire& wire){
      return new T(wire, I2C_ADDRESS);
    }

};

// template<typename...RegistrationT>
class I2CAutoDetector : public IAutoDetector {

  public:
    explicit I2CAutoDetector(TwoWire& wire,
      std::initializer_list<ISensorToAddressMapping*> entries):
      _wire(wire),
      mDetectionTable{entries}{};

    /**
     * @brief scan i2c bus for available sensors
     *
     * @param sensorList SensorList to which add the found sensors
     */
    void findSensors(SensorList& sensorList) override;

  private:
    TwoWire& _wire;
    std::vector<ISensorToAddressMapping*> mDetectionTable;
    ISensor* probeAddress(const byte& address) const;
};

#endif /* I2C_AUTO_DETECTOR_H */
