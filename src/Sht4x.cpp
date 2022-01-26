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
#include "Sht4x.h"
#include "SensirionCore.h"

uint16_t Sht4x::start() {
    _driver.begin(_wire);
    return HighLevelError::NoError;
}

uint16_t Sht4x::measure(DataPoint dataPoints[], const unsigned long timeStamp) {
    float temp;
    float humi;
    uint16_t error = _driver.measureHighPrecision(temp, humi);
    if (error) {
        return error;
    }
    dataPoints[0] =
        DataPoint(SensorId::SHT4X, Unit::TEMPERATURE_CELSIUS, temp, timeStamp);
    dataPoints[1] = DataPoint(
        SensorId::SHT4X, Unit::RELATIVE_HUMIDITY_PERCENTAGE, humi, timeStamp);
    return HighLevelError::NoError;
}

SensorId Sht4x::getSensorId() const {
    return SensorId::SHT4X;
}

size_t Sht4x::getNumberOfDataPoints() const {
    return 2;
}

unsigned long Sht4x::getMinimumMeasurementInterval() const {
    return 9;
}

void* Sht4x::getDriver() {
    return reinterpret_cast<void*>(&_driver);
}