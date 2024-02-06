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

#include "SensorWrappers/Sgp4x.h"
#include "SensirionCore.h"
#include "Sensirion_UPT_Core.h"

uint16_t Sgp41::start() {
    _driver.begin(_wire);
    return 0;
}

uint16_t Sgp41::measureAndWrite(Measurement measurements[],
                                const unsigned long timeStamp) {
    uint16_t srawVoc = 0;
    uint16_t srawNox = 0;

    uint16_t error =
        _driver.measureRawSignals(_defaultRh, _defaultT, srawVoc, srawNox);
    if (error) {
        return error;
    }
    MetaData metaData;
    metaData.deviceID = _sensorID;
    metaData.deviceType.sensorType = _sensorType;
    metaData.platform = DevicePlatform::WIRED;

    measurements[0].signalType = SignalType::RAW_VOC_INDEX;
    measurements[0].dataPoint.t_offset = timeStamp;
    measurements[0].dataPoint.value = static_cast<float>(srawVoc);
    measurements[0].metaData = metaData;

    measurements[1].signalType = SignalType::RAW_NOX_INDEX;
    measurements[1].dataPoint.t_offset = timeStamp;
    measurements[1].dataPoint.value = static_cast<float>(srawNox);
    measurements[1].metaData = metaData;

    return HighLevelError::NoError;
}

uint16_t Sgp41::initializationStep() {
    // Read serial No.
    uint16_t serialNo[3];
    uint16_t error = _driver.getSerialNumber(serialNo);
    if (error) {
        return error;
    }
    _sensorID = 0;
    _sensorID |= (static_cast<uint64_t>(serialNo[0]) << 16 * 2) |
                 (serialNo[1] << 16) | serialNo[2];

    uint16_t srawVoc;  // discarded during initialization
    error = _driver.executeConditioning(_defaultRh, _defaultT, srawVoc);
    return error;
}

SensorType Sgp41::getSensorType() const {
    return _sensorType;
}

size_t Sgp41::getNumberOfDataPoints() const {
    return 2;
}

unsigned long Sgp41::getMinimumMeasurementIntervalMs() const {
    return 1000;
}

bool Sgp41::requiresInitializationStep() const {
    return true;
}

unsigned long Sgp41::getInitializationIntervalMs() const {
    return 8000;
}

void* Sgp41::getDriver() {
    return reinterpret_cast<void*>(&_driver);
}

long Sgp41::readyStateDecayTimeMs() const {
    return 2000;
}
