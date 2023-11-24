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
#include "SensorWrappers/Scd30.h"
#include "SensirionCore.h"
#include "Sensirion_UPT_Core.h"

uint16_t Scd30::start() {
    _driver.begin(_wire, I2C_ADDRESS);
    return 0;
}

uint16_t Scd30::measureAndWrite(DataPoint dataPoints[],
                                const unsigned long timeStamp) {
    float co2Concentration;
    float temperature;
    float humidity;
    uint16_t error = _driver.blockingReadMeasurementData(co2Concentration,
                                                         temperature, humidity);
    if (error) {
        return error;
    }
    dataPoints[0] = DataPoint(SignalType::CO2_PARTS_PER_MILLION,
                              co2Concentration, timeStamp, sensorName(_id));
    dataPoints[1] = DataPoint(SignalType::TEMPERATURE_DEGREES_CELSIUS,
                              temperature, timeStamp, sensorName(_id));
    dataPoints[2] = DataPoint(SignalType::RELATIVE_HUMIDITY_PERCENTAGE,
                              humidity, timeStamp, sensorName(_id));
    return HighLevelError::NoError;
}

uint16_t Scd30::initializationStep() {
    // stop potentially previously started measurement
    _driver.stopPeriodicMeasurement();
    // Start Measurement
    uint16_t error = _driver.startPeriodicMeasurement(0);
    return error;
}

SensorID Scd30::getSensorId() const {
    return _id;
}

size_t Scd30::getNumberOfDataPoints() const {
    return 3;
}

unsigned long Scd30::getMinimumMeasurementIntervalMs() const {
    return 1500;
}

bool Scd30::requiresInitializationStep() const {
    return true;
}

void* Scd30::getDriver() {
    return reinterpret_cast<void*>(&_driver);
}
