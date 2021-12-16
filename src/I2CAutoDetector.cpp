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
#include "I2CAutoDetector.h"
#include "AutoDetectorErrors.h"
#include "Scd4x.h"
#include "Sen44.h"
#include "SensirionCore.h"
#include "Sfa3x.h"
#include "Svm40.h"

byte I2CAutoDetector::probeAddress(const byte& address) {
    _wire.beginTransmission(address);
    byte error = _wire.endTransmission();
    return error;
}

ISensor* I2CAutoDetector::createSensorFromAddress(const byte& address) {
    switch (address) {
        case (Scd4x::I2C_ADDRESS): {
            return new Scd4x(_wire);
        }
        case (Sen44::I2C_ADDRESS): {
            return new Sen44(_wire);
        }
        case (Sfa3x::I2C_ADDRESS): {
            return new Sfa3x(_wire);
        }
        case (Svm40::I2C_ADDRESS): {
            return new Svm40(_wire);
        }
        default: { return nullptr; }
    }
}

void I2CAutoDetector::findSensors(SensorList& sensorList) {
    for (byte address = 1; address < 127; address++) {
        byte probeFailed = probeAddress(address);
        if (probeFailed) {
            continue;
        }
        ISensor* pSensor = createSensorFromAddress(address);
        if (!pSensor) {
            continue;
        }
        uint16_t startFailed = pSensor->start();
        if (startFailed) {
            char errorMessage[256];
            Serial.print("Error trying to start() sensor instance: ");
            errorToString(startFailed, errorMessage, 256);
            Serial.println(errorMessage);
            delete pSensor;
            continue;
        }
        AutoDetectorError addFailed = sensorList.addSensor(pSensor);
        if (addFailed) {
            Serial.println("Error trying to add sensor instance "
                           "to sensorList.");
            delete pSensor;
        }
    }
}
