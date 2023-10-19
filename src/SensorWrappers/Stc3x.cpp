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

uint16_t Stc3x::measureAndWrite(DataPoint dataPoints[],
                                const unsigned long timeStamp) {
    uint16_t gasTicks;
    uint16_t temperatureTicks;

    uint16_t error =
        _driver.measureGasConcentration(gasTicks, temperatureTicks);
    if (error) {
        return error;
    }
    dataPoints[0] =
        DataPoint(SignalType::GAS_CONCENTRATION,
                  100 * (static_cast<float>(gasTicks) - 16384.0) / 32768.0,
                  timeStamp, sensorName(_id));
    dataPoints[1] = DataPoint(SignalType::TEMPERATURE_DEGREES_CELSIUS,
                              static_cast<float>(temperatureTicks) / 200.0,
                              timeStamp, sensorName(_id));
    return HighLevelError::NoError;
}

SensorID Stc3x::getSensorId() const {
    return _id;
}

size_t Stc3x::getNumberOfDataPoints() const {
    return 2;
}

unsigned long Stc3x::getMinimumMeasurementIntervalMs() const {
    return 1000;
}

void* Stc3x::getDriver() {
    return reinterpret_cast<void*>(&_driver);
}