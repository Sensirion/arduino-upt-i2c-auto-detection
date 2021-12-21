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
#include "Sfa3x.h"
#include "SensirionCore.h"

uint16_t Sfa3x::start() {
    _driver.begin(_wire);
    return _driver.startContinuousMeasurement();
}

uint16_t Sfa3x::measure(DataPoint dataPoints[], const unsigned long timeStamp) {
    int16_t hcho;
    int16_t humi;
    int16_t temp;

    uint16_t error = _driver.readMeasuredValues(hcho, humi, temp);
    if (error) {
        return error;
    }
    dataPoints[0] = DataPoint(SensorId::SFA3X, Unit::PARTS_PER_BILLION_HCHO,
                              static_cast<float>(hcho) / 5.0f, timeStamp);
    dataPoints[1] =
        DataPoint(SensorId::SFA3X, Unit::RELATIVE_HUMIDITY_PERCENTAGE,
                  static_cast<float>(humi) / 100.0f, timeStamp);
    dataPoints[2] = DataPoint(SensorId::SFA3X, Unit::TEMPERATURE_CELSIUS,
                              static_cast<float>(temp) / 200.0f, timeStamp);
    return HighLevelError::NoError;
}

SensorId Sfa3x::getSensorId() const {
    return SensorId::SFA3X;
}

size_t Sfa3x::getNumberOfDataPoints() const {
    return 3;
}

unsigned long Sfa3x::getMinimumMeasurementInterval() const {
    return 5000;
}

void* Sfa3x::getDriver() {
    return reinterpret_cast<void*>(&_driver);
}
