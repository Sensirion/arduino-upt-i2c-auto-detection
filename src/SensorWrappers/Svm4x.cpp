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

#include "SensorWrappers/Svm4x.h"
#include "SensirionCore.h"

uint16_t Svm4x::start() {
    _driver.begin(_wire);
    return 0;
}

uint16_t Svm4x::measureAndWrite(Measurement measurements[],
                                const unsigned long timeStamp) {
    float humidity;
    float temperature;
    float vocIndex;
    float noxIndex;
    uint16_t error =
        _driver.readMeasuredValues(humidity, temperature, vocIndex, noxIndex);
    if (error) {
        return error;
    }

    MetaData metaData;
    metaData.deviceID = _sensorID;
    metaData.deviceType.sensorType = _sensorType;
    metaData.platform = DevicePlatform::WIRED;

    measurements[0].signalType = SignalType::RELATIVE_HUMIDITY_PERCENTAGE;
    measurements[0].dataPoint.t_offset = timeStamp;
    measurements[0].dataPoint.value = humidity;
    measurements[0].metaData = metaData;

    measurements[1].signalType = SignalType::TEMPERATURE_DEGREES_CELSIUS;
    measurements[1].dataPoint.t_offset = timeStamp;
    measurements[1].dataPoint.value = temperature;
    measurements[1].metaData = metaData;

    measurements[2].signalType = SignalType::VOC_INDEX;
    measurements[2].dataPoint.t_offset = timeStamp;
    measurements[2].dataPoint.value = vocIndex;
    measurements[2].metaData = metaData;

    measurements[3].signalType = SignalType::NOX_INDEX;
    measurements[3].dataPoint.t_offset = timeStamp;
    measurements[3].dataPoint.value = noxIndex;
    measurements[3].metaData = metaData;

    return HighLevelError::NoError;
}

uint16_t Svm4x::initializationStep() {
    uint16_t error = _driver.stopMeasurement();
    if (error) {
        return error;
    }
    // Sensor Serial No.
    uint8_t serialNumber[32];
    uint8_t serialNumberSize = 32;
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
    return _driver.startMeasurement();
}

SensorType Svm4x::getSensorType() const {
    return _sensorType;
}

size_t Svm4x::getNumberOfDataPoints() const {
    return 4;
}

unsigned long Svm4x::getMinimumMeasurementIntervalMs() const {
    return 1000;
}

bool Svm4x::requiresInitializationStep() const {
    return true;
}

void* Svm4x::getDriver() {
    return reinterpret_cast<void*>(&_driver);
}
