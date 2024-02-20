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

#include "SensorWrappers/Scd4x.h"
#include "SensirionCore.h"
#include "Sensirion_UPT_Core.h"

uint16_t Scd4x::start() {
    _driver.begin(_wire);
    return 0;
}

uint16_t Scd4x::measureAndWrite(Measurement measurements[],
                                const unsigned long timeStamp) {
    uint16_t co2;
    float temp;
    float humi;
    uint16_t error = _driver.readMeasurement(co2, temp, humi);
    if (error) {
        return error;
    }

    MetaData metaData;
    metaData.deviceID = _sensorID;
    metaData.deviceType.sensorType = _sensorType;
    metaData.platform = DevicePlatform::WIRED;

    measurements[0].signalType = SignalType::CO2_PARTS_PER_MILLION;
    measurements[0].dataPoint.t_offset = timeStamp;
    measurements[0].dataPoint.value = static_cast<float>(co2);
    measurements[0].metaData = metaData;

    measurements[1].signalType = SignalType::TEMPERATURE_DEGREES_CELSIUS;
    measurements[1].dataPoint.t_offset = timeStamp;
    measurements[1].dataPoint.value = temp;
    measurements[1].metaData = metaData;

    measurements[2].signalType = SignalType::RELATIVE_HUMIDITY_PERCENTAGE;
    measurements[2].dataPoint.t_offset = timeStamp;
    measurements[2].dataPoint.value = humi;
    measurements[2].metaData = metaData;

    return HighLevelError::NoError;
}

uint16_t Scd4x::initializationStep() {
    // stop potentially previously started measurement
    uint16_t error = _driver.stopPeriodicMeasurement();
    if (error) {
        return error;
    }
    // Sensor Serial no.
    uint16_t word0, word1, word2;
    error = _driver.getSerialNumber(word0, word1, word2);
    if (error) {
        return error;
    }
    _sensorID = 0;
    _sensorID |=
        (static_cast<uint64_t>(word0) << 16 * 2) | (word1 << 16) | word2;

    // Start Measurement
    error = _driver.startPeriodicMeasurement();
    return error;
}

SensorType Scd4x::getSensorType() const {
    return _sensorType;
}

size_t Scd4x::getNumberOfDataPoints() const {
    return 3;
}

unsigned long Scd4x::getMinimumMeasurementIntervalMs() const {
    return 5000;
}

bool Scd4x::requiresInitializationStep() const {
    return true;
}

void* Scd4x::getDriver() {
    return reinterpret_cast<void*>(&_driver);
}
