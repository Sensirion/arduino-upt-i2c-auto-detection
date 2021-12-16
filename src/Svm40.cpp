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
#include "Svm40.h"
#include "SensirionCore.h"

uint16_t Svm40::start() {
    _driver.begin(_wire);
    // Start Measurement
    return _driver.startContinuousMeasurement();
}

uint16_t Svm40::measure(DataPoint dataPoints[], const unsigned long timeStamp) {
    int16_t vocIndex;
    int16_t humidity;
    int16_t temperature;
    uint16_t error =
        _driver.readMeasuredValuesAsIntegers(vocIndex, humidity, temperature);
    if (error) {
        return error;
    }
    dataPoints[0] = DataPoint(SensorId::SVM40, Unit::VOC_INDEX,
                              static_cast<float>(vocIndex) / 10.0f, timeStamp);
    dataPoints[1] =
        DataPoint(SensorId::SVM40, Unit::RELATIVE_HUMIDITY_PERCENTAGE,
                  static_cast<float>(humidity) / 100.0f, timeStamp);
    dataPoints[2] =
        DataPoint(SensorId::SVM40, Unit::TEMPERATURE_CELSIUS,
                  static_cast<float>(temperature) / 200.0f, timeStamp);
    return HighLevelError::NoError;
}

SensorId Svm40::getSensorId() const {
    return SensorId::SVM40;
}

size_t Svm40::getNumberOfDataPoints() const {
    return 3;
}

unsigned long Svm40::getMinimumMeasurementInterval() const {
    return 1000;
}
