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
#include "SensorWrappers/Svm4x.h"
#include "SensirionCore.h"

uint16_t Svm4x::start() {
    _driver.begin(_wire);
    uint16_t error = _driver.deviceReset();
    if (error) {
        return error;
    }
    // Start Measurement
    return _driver.startMeasurement();
}

uint16_t Svm4x::measureAndWrite(DataPoint dataPoints[],
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
    dataPoints[0] = DataPoint(SignalType::RELATIVE_HUMIDITY_PERCENTAGE,
                              humidity, timeStamp, sensorName(_id));
    dataPoints[1] = DataPoint(SignalType::TEMPERATURE_DEGREES_CELSIUS,
                              temperature, timeStamp, sensorName(_id));
    dataPoints[2] =
        DataPoint(SignalType::VOC_INDEX, vocIndex, timeStamp, sensorName(_id));
    dataPoints[3] =
        DataPoint(SignalType::NOX_INDEX, noxIndex, timeStamp, sensorName(_id));

    return HighLevelError::NoError;
}

SensorID Svm4x::getSensorId() const {
    return _id;
}

size_t Svm4x::getNumberOfDataPoints() const {
    return 4;
}

unsigned long Svm4x::getMinimumMeasurementIntervalMs() const {
    return 1000;
}

void* Svm4x::getDriver() {
    return reinterpret_cast<void*>(&_driver);
}
