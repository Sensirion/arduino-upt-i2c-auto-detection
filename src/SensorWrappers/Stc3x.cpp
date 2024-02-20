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

#include "SensorWrappers/Stc3x.h"
#include "SensirionCore.h"
#include "Sensirion_UPT_Core.h"

uint16_t Stc3x::start() {
    _driver.begin(_wire);
    uint16_t error = _driver.setBinaryGas(0x0001);
    return error;
}

uint16_t Stc3x::measureAndWrite(Measurement measurements[],
                                const unsigned long timeStamp) {
    uint16_t gasTicks;
    uint16_t temperatureTicks;

    uint16_t error =
        _driver.measureGasConcentration(gasTicks, temperatureTicks);
    if (error) {
        return error;
    }

    MetaData metaData;
    metaData.deviceID = _sensorID;
    metaData.deviceType.sensorType = _sensorType;
    metaData.platform = DevicePlatform::WIRED;

    measurements[0].signalType =
        SignalType::GAS_CONCENTRATION_VOLUME_PERCENTAGE;
    measurements[0].dataPoint.t_offset = timeStamp;
    measurements[0].dataPoint.value =
        100 * (static_cast<float>(gasTicks) - 16384.0) / 32768.0;
    measurements[0].metaData = metaData;

    measurements[1].signalType = SignalType::TEMPERATURE_DEGREES_CELSIUS;
    measurements[1].dataPoint.t_offset = timeStamp;
    measurements[1].dataPoint.value =
        static_cast<float>(temperatureTicks) / 200.0;
    measurements[1].metaData = metaData;

    return HighLevelError::NoError;
}

uint16_t Stc3x::initializationStep() {
    uint16_t error = _driver.prepareProductIdentifier();
    if (error) {
        return error;
    }

    uint32_t productNumber;
    uint8_t serialNumberRaw[32];
    uint8_t serialNumberSize = 32;

    error = _driver.readProductIdentifier(productNumber, serialNumberRaw,
                                          serialNumberSize);
    if (error) {
        return error;
    }
    // Sensor Product version
    /**
     * See
     * https://sensirion.com/media/documents/7B1D0EA7/61652CD0/Sensirion_Thermal_Conductivity_Datasheet_STC31_D1_1.pdf
     * Section 3.3.13
     */
    const uint32_t stc31ProductNumber = 0x08010301;
    const uint32_t mask = 0xFFFFFF00;
    const uint32_t maskedProductNo = productNumber & mask;
    const uint32_t maskedSTC31ProductNo = stc31ProductNumber & mask;
    if (maskedSTC31ProductNo == maskedProductNo) {
        _sensorType = SensorType::STC31;
    }  // else keep default STC3X

    // Sensor Serial No
    _sensorID = 0;
    _sensorID |= (uint64_t)serialNumberRaw[0] << 56 |
                 (uint64_t)serialNumberRaw[1] << 48 |
                 (uint64_t)serialNumberRaw[2] << 40 |
                 (uint64_t)serialNumberRaw[3] << 32 |
                 (uint64_t)serialNumberRaw[4] << 24 |
                 (uint64_t)serialNumberRaw[5] << 16 |
                 (uint64_t)serialNumberRaw[6] << 8 |
                 (uint64_t)serialNumberRaw[7];

    // Select gas mode
    /**
     * See
     * https://sensirion.com/media/documents/7B1D0EA7/61652CD0/Sensirion_Thermal_Conductivity_Datasheet_STC31_D1_1.pdf
     * Section 3.3.2
     *
     * Available Gas modes:
     *  0x0000: CO2 in N2   (range 0-100% vol.)
     *  0x0001: CO2 in Air  (range 0-100% vol.)
     *  0x0002: CO2 in N2   (range 0-25%  vol.)
     *  0x0003: CO2 in Air  (range 0-25%  vol.)
     */
    error = _driver.setBinaryGas(0x0003);
    if (error) {
        return error;
    }

    return HighLevelError::NoError;
}

SensorType Stc3x::getSensorType() const {
    return _sensorType;
}

size_t Stc3x::getNumberOfDataPoints() const {
    return 2;
}

unsigned long Stc3x::getMinimumMeasurementIntervalMs() const {
    return 1000;
}

bool Stc3x::requiresInitializationStep() const {
    return 1;
}

void* Stc3x::getDriver() {
    return reinterpret_cast<void*>(&_driver);
}
