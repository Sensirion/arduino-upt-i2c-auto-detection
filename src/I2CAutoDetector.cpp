#include "I2CAutoDetector.h"
#include "DriverConfig.h"
#include "SensirionCore.h"

#ifdef INCLUDE_SCD30_DRIVER
#include "SensorWrappers/Scd30.h"
#endif
#ifdef INCLUDE_SCD4X_DRIVER
#include "SensorWrappers/Scd4x.h"
#endif
#ifdef INCLUDE_SEN5X_DRIVER
#include "SensorWrappers/Sen5x.h"
#endif
#ifdef INCLUDE_SFA3X_DRIVER
#include "SensorWrappers/Sfa3x.h"
#endif
#ifdef INCLUDE_SGP4X_DRIVER
#include "SensorWrappers/Sgp4x.h"
#endif
#ifdef INCLUDE_SHT4X_DRIVER
#include "SensorWrappers/Sht4x.h"
#endif
#ifdef INCLUDE_STC3X_DRIVER
#include "SensorWrappers/Stc3x.h"
#endif
#ifdef INCLUDE_SVM4X_DRIVER
#include "SensorWrappers/Svm4x.h"
#endif
#ifdef INCLUDE_SEN66_DRIVER
#include "SensorWrappers/Sen66.h"
#endif

void I2CAutoDetector::findSensors(SensorList& sensorList) {
    for (byte address = 1; address < 127; address++) {
        auto sensor = probeAddress(address);
        if (sensor == nullptr) {
            continue;
        }
        if (sensorList.containsSensor(sensor->getSensorType())) {
            continue;
        }

        sensorList.addSensor(sensor);
    }
}

/**
 * Private methods
 */

ISensor* I2CAutoDetector::probeAddress(const byte& address) const {
    _wire.beginTransmission(address);
    const byte error = _wire.endTransmission();
    if (error){
        return nullptr;
    }
    auto iterator = std::find_if(mDetectionTable.begin(), mDetectionTable.end(),
    [address](const ISensorToAddressMapping* mapping) {
        return mapping->getI2cAddress() == address;
    }
    );
    // TODO complete this
    return nullptr;
}

