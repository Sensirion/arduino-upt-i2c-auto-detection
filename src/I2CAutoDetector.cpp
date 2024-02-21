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

byte I2CAutoDetector::probeAddress(const byte& address) {
    _wire.beginTransmission(address);
    byte error = _wire.endTransmission();
    return error;
}

ISensor* I2CAutoDetector::createSensorFromAddress(const byte& address) {
    switch (address) {
#ifdef INCLUDE_SCD30_DRIVER
        case (Scd30::I2C_ADDRESS): {
            return new Scd30(_wire);
        }
#endif
#ifdef INCLUDE_SCD4X_DRIVER
        case (Scd4x::I2C_ADDRESS): {
            return new Scd4x(_wire);
        }
#endif
#ifdef INCLUDE_SEN5X_DRIVER
        case (Sen5x::I2C_ADDRESS): {
            return new Sen5x(_wire);
        }
#endif
#ifdef INCLUDE_SFA3X_DRIVER
        case (Sfa3x::I2C_ADDRESS): {
            return new Sfa3x(_wire);
        }
#endif
#ifdef INCLUDE_SGP4X_DRIVER
        case (Sgp41::I2C_ADDRESS): {
            return new Sgp41(_wire);
        }
#endif
#ifdef INCLUDE_SHT4X_DRIVER
        case (Sht4x::I2C_ADDRESS): {
            return new Sht4x(_wire);
        }
#endif
#ifdef INCLUDE_STC3X_DRIVER
        case (Stc3x::I2C_ADDRESS): {
            return new Stc3x(_wire);
        }
#endif
#ifdef INCLUDE_SVM4X_DRIVER
        case (Svm4x::I2C_ADDRESS): {
            return new Svm4x(_wire);
        }
#endif
        default: {
            return nullptr;
        }
    }
}

void I2CAutoDetector::findSensors(SensorList& sensorList) {
    for (byte address = 1; address < 127; address++) {
        byte probeFailed = probeAddress(address);
        if (probeFailed) {
            continue;
        }
        ISensor* pSensor = createSensorFromAddress(address);
        if (!pSensor) {
            continue;
        }
        sensorList.addSensor(pSensor);
    }
}
