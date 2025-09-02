#ifndef I2C_AUTO_DETECTOR_H
#define I2C_AUTO_DETECTOR_H

#include "IAutoDetector.h"
#include "I2cSensorMapping.h"
#include <Wire.h>
#include <array>

namespace sensirion::upt::i2c_autodetect{

/// Class to search registered sensors on the i2c bus
///
/// @tparam ...SensorMappingT Address to sensor type mapping
template<typename...SensorMappingT>
class I2CAutoDetector : public IAutoDetector {

  public:
    /// defines the number of configured sensors
    static constexpr size_t CONFIGURED_SENSORS = sizeof...(SensorMappingT);


    explicit I2CAutoDetector(TwoWire& wire):
      _wire(wire),
      mDetectionTable{createMappingInstance<SensorMappingT>(wire)...}{};

    virtual ~I2CAutoDetector() {
      for( auto entry : mDetectionTable){
        delete entry;
      }
    }

    /** 
     * Get the number of configured sensors
     * 
     * @return The number of sensors that can be configured
    */
    virtual size_t configuredSensorsCount() const override{
      return CONFIGURED_SENSORS;
    }

    /**
     * @brief scan i2c bus for available sensors
     *
     * @param sensorList SensorList to which add the found sensors
     */
    virtual void findSensors(SensorList& sensorList) override {
      for (auto tableEntry:mDetectionTable){      
        _wire.beginTransmission(tableEntry->getI2cAddress());
        const byte error = _wire.endTransmission();
        if (error){
            continue;
        }
        sensorList.addSensor(&tableEntry->getSensor());
      }
    }
  

  private:
    using DetectableSensorsT = std::array<ISensorToAddressMapping*, 
          sizeof...(SensorMappingT)>;

    DetectableSensorsT mDetectionTable;

    TwoWire& _wire;

    /// Helper funciton to implement the initialisation of the detection table
    ///
    /// Required for fold expression
    /// @tparam T The type of the SensorMapping to be constructed.
    /// @param wire Parameter to the constructor of T
    /// @return A pointer of type ISensorToAddressMapping
    template<typename T>
    ISensorToAddressMapping* createMappingInstance(TwoWire& wire){
      return new T(wire);
    }
};
} // namespace sensirion::upt::i2c_autodetect 

#endif /* I2C_AUTO_DETECTOR_H */
