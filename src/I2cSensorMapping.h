#ifndef I2C_SENSOR_MAPPING_H
#define I2C_SENSOR_MAPPING_H

#include "IAutoDetector.h"
#include <Wire.h>
#include <array>
#include <vector>

namespace sensirion::upt::i2c_autodetect{

/// This is the interface of an entry in the registration table
/// that allows to map sensors to i2c addresses.
/// We define this interface in order to allow type erasure.
struct ISensorToAddressMapping{
  using SensorRef = ISensor&;

  virtual uint8_t getI2cAddress() const = 0;
  virtual SensorRef getSensor() = 0;
  virtual ~ISensorToAddressMapping(){}
};

/// Defines an entry int the sensor registation table.
///
/// @tparam address The i2c address
/// @tparam SensorT The sensor classe that lists on the specified i2c address
template<uint8_t address, typename SensorT>
struct SensorToAddressMapping: ISensorToAddressMapping{

  static constexpr uint8_t I2C_ADDRESS = address;

  explicit SensorToAddressMapping(TwoWire& wire):
    mSensor(wire, I2C_ADDRESS){}

  /// No copy, assign, move
  SensorToAddressMapping(const SensorToAddressMapping& other) = delete;
  SensorToAddressMapping(const SensorToAddressMapping&& other) = delete;
  SensorToAddressMapping& operator=(const SensorToAddressMapping& other) = delete;

  
  virtual uint8_t getI2cAddress()  const override{
    return I2C_ADDRESS;
  }

  virtual SensorRef getSensor() override{
    return mSensor;
  }

  private:

    SensorT mSensor;
};
} // namespace sensirion::upt::i2c_autodetect 

#endif // I2C_SENSOR_MAPPING_H
