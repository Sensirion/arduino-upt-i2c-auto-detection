#ifndef DRIVER_CONFIG_H
#define DRIVER_CONFIG_H

#include "I2cSensorMapping.h"
#include "SensorWrappers/Scd30.h"
#include "SensorWrappers/Scd4x.h"
#include "SensorWrappers/Sen5x.h"
#include "SensorWrappers/Sen62.h"
#include "SensorWrappers/Sen63c.h"
#include "SensorWrappers/Sen65.h"
#include "SensorWrappers/Sen66.h"
#include "SensorWrappers/Sen68.h"
#include "SensorWrappers/Sen69c.h"
#include "SensorWrappers/Sfa3x.h"
#include "SensorWrappers/Sgp4x.h"
#include "SensorWrappers/Sht4x.h"
#include "SensorWrappers/Stc3x.h"
#include "SensorWrappers/Stcc4.h"
#include "SensorWrappers/Svm4x.h"

namespace sensirion::upt::i2c_autodetect {

using Scd30Mapping = SensorToAddressMapping<0x61, Scd30>;
using Scd4xMapping = SensorToAddressMapping<0x62, Scd4x>;
using Sen5xMapping = SensorToAddressMapping<0x69, Sen5x>;
using Sen62Mapping = SensorToAddressMapping<0x6b, Sen62>;
using Sen63cMapping = SensorToAddressMapping<0x6b, Sen63c>;
using Sen65Mapping = SensorToAddressMapping<0x6b, Sen65>;
using Sen66Mapping = SensorToAddressMapping<0x6b, Sen66>;
using Sen68Mapping = SensorToAddressMapping<0x6b, Sen68>;
using Sen69cMapping = SensorToAddressMapping<0x6b, Sen69c>;
using Sfa3xMapping = SensorToAddressMapping<0x5d, Sfa3x>;
using Sgp41Mapping = SensorToAddressMapping<0x59, Sgp41>;
using Sht4xMapping = SensorToAddressMapping<0x44, Sht4x>;
using Stc3xMapping = SensorToAddressMapping<0x29, Stc3x>;
using Svm4xMapping = SensorToAddressMapping<0x6a, Svm4x>;
using Stcc4Mapping = SensorToAddressMapping<0x64, Stcc4>;

typedef I2CAutoDetector<Scd30Mapping, Scd4xMapping, Sen5xMapping, Sen62Mapping,
                        Sen63cMapping, Sen65Mapping, Sen66Mapping, Sen68Mapping,
                        Sen69cMapping, Sfa3xMapping, Sgp41Mapping, Sht4xMapping,
                        Stc3xMapping, Svm4xMapping, Stcc4Mapping>
    DefaultI2cDetector;
}  // namespace sensirion::upt::i2c_autodetect

#endif /* DRIVER_CONFIG_H */
