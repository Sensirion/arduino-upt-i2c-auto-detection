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
#include "SensorWrappers/Sgp41.h"
#include "SensirionCore.h"
#include "Sensirion_UPT_Core.h"

uint16_t Sgp41::start() {
    _driver.begin(_wire);
    return 0;
}

uint16_t Sgp41::measureAndWrite(DataPoint dataPoints[],
                                const unsigned long timeStamp) {
    uint16_t srawVoc = 0;
    uint16_t srawNox = 0;

    uint16_t error =
        _driver.measureRawSignals(_defaultRh, _defaultT, srawVoc, srawNox);
    if (error) {
        return error;
    }
    dataPoints[0] =
        DataPoint(SignalType::VOC_INDEX, static_cast<float>(srawVoc), timeStamp,
                  sensorName(_id));
    dataPoints[1] =
        DataPoint(SignalType::NOX_INDEX, static_cast<float>(srawNox), timeStamp,
                  sensorName(_id));
    return HighLevelError::NoError;
}

uint16_t Sgp41::initializationStep() {
    uint16_t srawVoc;  // discarded during initialization
    uint16_t error =
        _driver.executeConditioning(_defaultRh, _defaultT, srawVoc);
    return error;
}

SensorID Sgp41::getSensorId() const {
    return _id;
}

size_t Sgp41::getNumberOfDataPoints() const {
    return 2;
}

unsigned long Sgp41::getMinimumMeasurementIntervalMs() const {
    return 1000;
}

unsigned long Sgp41::getInitializationSteps() const {
    return 10;
}

unsigned long Sgp41::getInitializationIntervalMs() const {
    return 1000;
}

void* Sgp41::getDriver() {
    return reinterpret_cast<void*>(&_driver);
}
