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

void I2CAutoDetector::findSensors(SensorList& sensorList) {
    Serial.println("Searching for Sensors..");
    int nSensors = 0;
    for (byte address = 1; address < 127; address++) {
        _wire.beginTransmission(address);
        byte error = _wire.endTransmission();

        if (error == 0) {
            Serial.print("Found Address 0x");
            if (address < 16) {
                Serial.print('0');
            }
            Serial.print(address, HEX);
            Serial.print(": ");

            switch (address) {
                case (0x62): {
                    Serial.print("Adding SCD4X .. ");
                    ISensor* pSensor = new Scd4x(_wire);
                    error = pSensor->start();
                    if (error) {
                        Serial.println("ERROR: Failed to Start Sensor!");
                        delete pSensor;
                        break;
                    }
                    error = sensorList.addSensor(pSensor);
                    if (error) {
                        Serial.println("ERROR: Failed to Add Sensor!");
                        delete pSensor;
                    } else {
                        nSensors++;
                        Serial.println("Success!");
                    }
                    break;
                }
                    // further cases

                default:
                    Serial.println("No matching sensor!");
            }
        }
    }
    if (nSensors == 0) {
        Serial.println("No sensors detected!");
    }
}
