/*
 * Copyright (c) 2023, Sensirion AG
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
    return 0;
}

uint16_t Sen5x::measureAndWrite(Measurement measurements[],
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

    MetaData metaData;
    metaData.deviceID = _sensorID;
    metaData.deviceType.sensorType = _sensorType;
    metaData.platform = DevicePlatform::WIRED;

    // Versions 50, 54 and 55
    measurements[0].signalType = SignalType::PM1P0_MICRO_GRAMM_PER_CUBIC_METER;
    measurements[0].dataPoint.t_offset = timeStamp;
    measurements[0].dataPoint.value = massConcentrationPm1p0;
    measurements[0].metaData = metaData;

    measurements[1].signalType = SignalType::PM2P5_MICRO_GRAMM_PER_CUBIC_METER;
    measurements[1].dataPoint.t_offset = timeStamp;
    measurements[1].dataPoint.value = massConcentrationPm2p5;
    measurements[1].metaData = metaData;

    measurements[2].signalType = SignalType::PM4P0_MICRO_GRAMM_PER_CUBIC_METER;
    measurements[2].dataPoint.t_offset = timeStamp;
    measurements[2].dataPoint.value = massConcentrationPm4p0;
    measurements[2].metaData = metaData;

    measurements[3].signalType = SignalType::PM10P0_MICRO_GRAMM_PER_CUBIC_METER;
    measurements[3].dataPoint.t_offset = timeStamp;
    measurements[3].dataPoint.value = massConcentrationPm10p0;
    measurements[3].metaData = metaData;

    // Verions 54, 55
    if (_sensorType == SensorType::SEN54 or _sensorType == SensorType::SEN55) {
        measurements[4].signalType = SignalType::RELATIVE_HUMIDITY_PERCENTAGE;
        measurements[4].dataPoint.t_offset = timeStamp;
        measurements[4].dataPoint.value = ambientHumidity;
        measurements[4].metaData = metaData;

        measurements[5].signalType = SignalType::TEMPERATURE_DEGREES_CELSIUS;
        measurements[5].dataPoint.t_offset = timeStamp;
        measurements[5].dataPoint.value = ambientTemperature;
        measurements[5].metaData = metaData;

        measurements[6].signalType = SignalType::VOC_INDEX;
        measurements[6].dataPoint.t_offset = timeStamp;
        measurements[6].dataPoint.value = vocIndex;
        measurements[6].metaData = metaData;
    }
    // Version 55
    if (_sensorType == SensorType::SEN55) {
        measurements[7].signalType = SignalType::NOX_INDEX;
        measurements[7].dataPoint.t_offset = timeStamp;
        measurements[7].dataPoint.value = noxIndex;
        measurements[7].metaData = metaData;
    }
    return HighLevelError::NoError;
}

uint16_t Sen5x::initializationStep() {
    // stop potentially previously started measurement
    uint16_t error = _driver.stopMeasurement();
    if (error) {
        return error;
    }

    // Get sensor version (SEN50/SEN54/SEN55)
    error = _determineSensorVersion();
    if (error) {
        return error;
    }

    // Get sensor unique ID (last 8 chars of serial no.)
    uint8_t serialNumberSize = 32;
    unsigned char serialNumber[serialNumberSize];
    error = _driver.getSerialNumber(serialNumber, serialNumberSize);
    if (error) {
        return error;
    }
    size_t actualLen = strlen((const char*)serialNumber);
    size_t numBytesToCopy = min(8, (int)actualLen);
    _sensorID = 0;
    for (int i = 0; i < numBytesToCopy - 1; i++) {
        _sensorID |= (serialNumber[actualLen - numBytesToCopy - 1 + i]);
        _sensorID = _sensorID << 8;
    }
    _sensorID |= serialNumber[actualLen - 1];

    // Start Measurement
    error = _driver.startMeasurement();
    if (error) {
        return error;
    }
    error = _determineSensorVersion();
    return error;
}

SensorType Sen5x::getSensorType() const {
    return _sensorType;
}

size_t Sen5x::getNumberOfDataPoints() const {
    switch (_sensorType) {
        case SensorType::SEN5X:
            return 4;
        case SensorType::SEN50:
            return 4;
        case SensorType::SEN54:
            return 7;
        case SensorType::SEN55:
            return 8;
        default:
            return 0;
    }
}

unsigned long Sen5x::getMinimumMeasurementIntervalMs() const {
    return 1000;
}

bool Sen5x::requiresInitializationStep() const {
    return true;
}

void* Sen5x::getDriver() {
    return reinterpret_cast<void*>(&_driver);
}

uint16_t Sen5x::_determineSensorVersion() {
    uint8_t sensorNameSize = 32;
    unsigned char sensorNameStr[sensorNameSize];
    uint16_t error = _driver.getProductName(sensorNameStr, sensorNameSize);

    if (error) {
        return error;
    }

    if (strcmp(reinterpret_cast<const char*>(sensorNameStr), "SEN50") == 0) {
        _sensorType = SensorType::SEN50;
    } else if (strcmp(reinterpret_cast<const char*>(sensorNameStr), "SEN54") ==
               0) {
        _sensorType = SensorType::SEN54;
    } else if (strcmp(reinterpret_cast<const char*>(sensorNameStr), "SEN55") ==
               0) {
        _sensorType = SensorType::SEN55;
    } else {
        _sensorType = SensorType::SEN5X;
    }
    return 0;
}
