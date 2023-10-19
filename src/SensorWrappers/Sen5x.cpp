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
#include "SensorWrappers/Sen5x.h"
#include "SensirionCore.h"

uint16_t Sen5x::start() {
    _driver.begin(_wire);
    // stop potentially previously started measurement
    uint16_t error = _driver.deviceReset();
    if (error) {
        return error;
    }
    // Start Measurement
    error = _driver.startMeasurement();
    if (error) {
        return error;
    }
    delay(1000);  // Need to wait for first measurement to finish (~1s) in order
                  // to be able to determine the sensor version.
    // determine SEN5X version
    error = _determineSensorVersion();
    return error;
}

uint16_t Sen5x::measureAndWrite(DataPoint dataPoints[],
                                const unsigned long timeStamp) {
    uint16_t error = 0;

    // Read Measurement
    float massConcentrationPm1p0;
    float massConcentrationPm2p5;
    float massConcentrationPm4p0;
    float massConcentrationPm10p0;
    float ambientHumidity;
    float ambientTemperature;
    float vocIndex;
    float noxIndex;

    error = _driver.readMeasuredValues(
        massConcentrationPm1p0, massConcentrationPm2p5, massConcentrationPm4p0,
        massConcentrationPm10p0, ambientHumidity, ambientTemperature, vocIndex,
        noxIndex);

    if (error) {
        return error;
    }
    // Versions 50, 54 and 55
    dataPoints[0] = DataPoint(SignalType::PM1P0_MICRO_GRAMM_PER_CUBIC_METER,
                              static_cast<float>(massConcentrationPm1p0),
                              timeStamp, sensorName(_id));
    dataPoints[1] = DataPoint(SignalType::PM2P5_MICRO_GRAMM_PER_CUBIC_METER,
                              static_cast<float>(massConcentrationPm2p5),
                              timeStamp, sensorName(_id));
    dataPoints[2] = DataPoint(SignalType::PM4P0_MICRO_GRAMM_PER_CUBIC_METER,
                              static_cast<float>(massConcentrationPm4p0),
                              timeStamp, sensorName(_id));
    dataPoints[3] = DataPoint(SignalType::PM10P0_MICRO_GRAMM_PER_CUBIC_METER,
                              static_cast<float>(massConcentrationPm10p0),
                              timeStamp, sensorName(_id));

    // Verions 54, 55
    if (_version == SensorVersion::SEN54 or _version == SensorVersion::SEN55) {
        dataPoints[4] = DataPoint(SignalType::RELATIVE_HUMIDITY_PERCENTAGE,
                                  ambientHumidity, timeStamp, sensorName(_id));
        dataPoints[5] =
            DataPoint(SignalType::TEMPERATURE_DEGREES_CELSIUS,
                      ambientTemperature, timeStamp, sensorName(_id));
        dataPoints[6] = DataPoint(SignalType::VOC_INDEX, vocIndex, timeStamp,
                                  sensorName(_id));
    }
    // Version 55
    if (_version == SensorVersion::SEN55) {
        dataPoints[7] = DataPoint(SignalType::NOX_INDEX, noxIndex, timeStamp,
                                  sensorName(_id));
    }
    return HighLevelError::NoError;
}

SensorID Sen5x::getSensorId() const {
    return _id;
}

size_t Sen5x::getNumberOfDataPoints() const {
    switch (_version) {
        case SensorVersion::SEN50:
            return 4;
        case SensorVersion::SEN54:
            return 7;  // sure about that?
        case SensorVersion::SEN55:
            return 8;
        default:
            return 0;
    }
}

unsigned long Sen5x::getMinimumMeasurementIntervalMs() const {
    return 1000;
}

void* Sen5x::getDriver() {
    return reinterpret_cast<void*>(&_driver);
}

uint16_t Sen5x::_determineSensorVersion() {
    uint16_t error = 0;
    // Test measurement
    // Read Measurement
    float massConcentrationPm1p0;
    float massConcentrationPm2p5;
    float massConcentrationPm4p0;
    float massConcentrationPm10p0;
    float ambientHumidity;
    float ambientTemperature;
    float vocIndex;
    float noxIndex;

    error = _driver.readMeasuredValues(
        massConcentrationPm1p0, massConcentrationPm2p5, massConcentrationPm4p0,
        massConcentrationPm10p0, ambientHumidity, ambientTemperature, vocIndex,
        noxIndex);
    if (isnan(massConcentrationPm2p5)) {  // Sanity check
        return HighLevelError::SensorSpecificError;
    }
    if (!isnan(noxIndex)) {  // Only SEN55 has NOX data
        _version = SensorVersion::SEN55;
    } else if (!isnan(vocIndex)) {
        _version = SensorVersion::SEN54;  // Only SEN54 has VOC but no NOX
    } else {
        _version =
            SensorVersion::SEN50;  // SEN50 has pm values but no VOC nor NOX
    }
    return error;
}
