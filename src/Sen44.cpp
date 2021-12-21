/*
 * Copyright (c) 2021, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "Sen44.h"
#include "SensirionCore.h"

uint16_t Sen44::start() {
    _driver.begin(_wire);
    // stop potentially previously started measurement
    uint16_t error = _driver.deviceReset();
    if (error) {
        return error;
    }
    // Start Measurement
    error = _driver.startMeasurement();
    return error;
}

uint16_t Sen44::measure(DataPoint dataPoints[], const unsigned long timeStamp) {
    uint16_t massConcentrationPm1p0;
    uint16_t massConcentrationPm2p5;
    uint16_t massConcentrationPm4p0;
    uint16_t massConcentrationPm10p0;
    float vocIndex;
    float ambientHumidity;
    float ambientTemperature;
    uint16_t error = _driver.readMeasuredMassConcentrationAndAmbientValues(
        massConcentrationPm1p0, massConcentrationPm2p5, massConcentrationPm4p0,
        massConcentrationPm10p0, vocIndex, ambientHumidity, ambientTemperature);
    if (error) {
        return error;
    }
    dataPoints[0] =
        DataPoint(SensorId::SEN44, Unit::MASS_CONCENTRATION_PM1P0,
                  static_cast<float>(massConcentrationPm1p0), timeStamp);
    dataPoints[1] =
        DataPoint(SensorId::SEN44, Unit::MASS_CONCENTRATION_PM2P5,
                  static_cast<float>(massConcentrationPm2p5), timeStamp);
    dataPoints[2] =
        DataPoint(SensorId::SEN44, Unit::MASS_CONCENTRATION_PM4P0,
                  static_cast<float>(massConcentrationPm4p0), timeStamp);
    dataPoints[3] =
        DataPoint(SensorId::SEN44, Unit::MASS_CONCENTRATION_PM10P0,
                  static_cast<float>(massConcentrationPm10p0), timeStamp);
    dataPoints[4] =
        DataPoint(SensorId::SEN44, Unit::VOC_INDEX, vocIndex, timeStamp);
    dataPoints[5] =
        DataPoint(SensorId::SEN44, Unit::RELATIVE_HUMIDITY_PERCENTAGE,
                  ambientHumidity, timeStamp);
    dataPoints[6] = DataPoint(SensorId::SEN44, Unit::TEMPERATURE_CELSIUS,
                              ambientTemperature, timeStamp);

    return HighLevelError::NoError;
}

SensorId Sen44::getSensorId() const {
    return SensorId::SEN44;
}

size_t Sen44::getNumberOfDataPoints() const {
    return 7;
}

unsigned long Sen44::getMinimumMeasurementInterval() const {
    return 1000;
}

void* Sen44::getDriver() {
    return reinterpret_cast<void*>(&_driver);
}
