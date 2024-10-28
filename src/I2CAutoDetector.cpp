#include "I2CAutoDetector.h"
#include "AutoDetectorErrors.h"
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

void I2CAutoDetector::findSensors(SensorList& sensorList) {
    for (byte address = 1; address < 127; address++) {
        auto st = probeAddress(address);
        if (st == SensorType::UNDEFINED) {
            continue;
        }
        if (sensorList.containsSensor(st)) {
            continue;
        }

        ISensor* pSensor = createSensorFromSensorType(st);
        if (!pSensor) {
            continue;
        }
        sensorList.addSensor(pSensor);
    }
}

/**
 * Private methods
 */

SensorType I2CAutoDetector::probeAddress(const byte& address) {
    _wire.beginTransmission(address);
    byte error = _wire.endTransmission();
    if (error){
        return SensorType::UNDEFINED;
    }
    return getSensorTypeFromAddress(address);
}

ISensor* I2CAutoDetector::createSensorFromSensorType(SensorType sensor_type) {
    switch (sensor_type) {
        case (SensorType::SCD30): {
            return new Scd30(_wire);
        }
        case (SensorType::SCD4X): {
            return new Scd4x(_wire);
        }

        case (SensorType::SEN5X): {
            return new Sen5x(_wire);
        }
        case (SensorType::SFA3X): {
            return new Sfa3x(_wire);
        }
        case (SensorType::SGP41): {
            return new Sgp41(_wire);
        }
        case (SensorType::SHT4X): {
            return new Sht4x(_wire);
        }
        case (SensorType::STC3X): {
            return new Stc3x(_wire);
        }
        case (SensorType::SVM4X): {
            return new Svm4x(_wire);
        }
        default: {
            return nullptr;
        }
    }
}


SensorType I2CAutoDetector::getSensorTypeFromAddress(const byte address) {
    switch (address) {
#ifdef INCLUDE_SCD30_DRIVER
        case (Scd30::I2C_ADDRESS): {
            return SensorType::SCD30;
        }
#endif
#ifdef INCLUDE_SCD4X_DRIVER
        case (Scd4x::I2C_ADDRESS): {
            return SensorType::SCD4X;
        }
#endif
#ifdef INCLUDE_SEN5X_DRIVER
        case (Sen5x::I2C_ADDRESS): {
            return SensorType::SEN5X;
        }
#endif
#ifdef INCLUDE_SFA3X_DRIVER
        case (Sfa3x::I2C_ADDRESS): {
            return SensorType::SFA3X;
        }
#endif
#ifdef INCLUDE_SGP4X_DRIVER
        case (Sgp41::I2C_ADDRESS): {
            return SensorType::SGP41;
        }
#endif
#ifdef INCLUDE_SHT4X_DRIVER
        case (Sht4x::I2C_ADDRESS): {
            return SensorType::SHT4X;
        }
#endif
#ifdef INCLUDE_STC3X_DRIVER
        case (Stc3x::I2C_ADDRESS): {
            return SensorType::STC3X;
        }
#endif
#ifdef INCLUDE_SVM4X_DRIVER
        case (Svm4x::I2C_ADDRESS): {
            return SensorType::SVM4X;
        }
#endif
        default: {
            return SensorType::UNDEFINED;
        }
    }
}


