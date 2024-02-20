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

#include "SensorList.h"

size_t SensorList::_hashSensorType(SensorType sensorType) const {
    SensorHash sensorHash = SensorHash::_UNDEFINED;
    switch (sensorType) {
        case SensorType::SCD4X:
            sensorHash = SensorHash::_SCD4X;
            break;
        case SensorType::SFA3X:
            sensorHash = SensorHash::_SFA3X;
            break;
        case SensorType::SVM41:
        case SensorType::SVM4X:
            sensorHash = SensorHash::_SVM4X;
            break;
        case SensorType::SHT4X:
            sensorHash = SensorHash::_SHT4X;
            break;
        case SensorType::SAV45:
            sensorHash = SensorHash::_UNDEFINED;  // SAV45 not yet supported
            break;
        case SensorType::SEN50:
        case SensorType::SEN54:
        case SensorType::SEN55:
        case SensorType::SEN5X:
            sensorHash = SensorHash::_SEN5X;
            break;
        case SensorType::SCD30:
            sensorHash = SensorHash::_SCD30;
            break;
        case SensorType::STC31:
        case SensorType::STC3X:
            sensorHash = SensorHash::_STC3X;
            break;
        case SensorType::SGP41:
        case SensorType::SGP4X:
            sensorHash = SensorHash::_SGP4X;
            break;
        default:
            sensorHash = SensorHash::_UNDEFINED;
    }

    if (sensorHash == SensorHash::_UNDEFINED) {
        Serial.printf("Error: invalid sensor type %s encountered. Aborting!\n",
                      sensorLabel(sensorType));
        assert(0);
    }

    // Substracting 1 to hash from 0 up to and including SensorManager's
    // _MAX_NUM_SENSORS
    return static_cast<size_t>(sensorHash) - 1;
}

SensorList::SensorList(uint8_t numSensors) : _numSensors(numSensors) {
    _sensors = new SensorStateMachine*[_numSensors];
    for (size_t i = 0; i < _numSensors; i++) {
        _sensors[i] = nullptr;
    }
}

SensorList::~SensorList() {
    delete[] _sensors;
}

AutoDetectorError SensorList::addSensor(ISensor* pSensor) {
    // Check if we already have it
    SensorType newSensorType = pSensor->getSensorType();
    if (containsSensor(newSensorType)) {
        return NO_ERROR;
    }
    // Hash
    size_t hashIndex = _hashSensorType(newSensorType);
    // Insert
    if (_sensors[hashIndex] == nullptr) {
        _sensors[hashIndex] = new SensorStateMachine(pSensor);
        return NO_ERROR;
    }

    return SENSOR_LIST_ALREADY_CONTAINS_SENSOR_ERROR;
}

size_t SensorList::count() {
    size_t numberOfSensors = 0;
    for (int i = 0; i < _numSensors; ++i) {
        if (_sensors[i]) {
            ++numberOfSensors;
        }
    }
    return numberOfSensors;
}

size_t SensorList::getTotalNumberOfDataPoints() const {
    size_t totalNumberOfDataPoints = 0;
    for (int i = 0; i < _numSensors; ++i) {
        if (_sensors[i]) {
            totalNumberOfDataPoints +=
                _sensors[i]->getSensor()->getNumberOfDataPoints();
        }
    }
    return totalNumberOfDataPoints;
}

int SensorList::getLength() const {
    return _numSensors;
}

SensorStateMachine* SensorList::getSensorStateMachine(size_t i) {
    return _sensors[i];
}

ISensor* SensorList::getSensor(SensorType sensorType) const {
    size_t sensorHashIndex = _hashSensorType(sensorType);
    if (_sensors[sensorHashIndex]) {
        return _sensors[sensorHashIndex]->getSensor();
    }
    return nullptr;
}

bool SensorList::containsSensor(SensorType sensorType) const {
    size_t qHashIndex = _hashSensorType(sensorType);

    // Check (1) if a sensor of this type is known and (2) if the version
    // matches as well
    if (_sensors[qHashIndex] &&
        _sensors[qHashIndex]->getSensor()->getSensorType() == sensorType) {
        return true;
    }

    return false;
}

void SensorList::removeLostSensors() {
    for (size_t i = 0; i < _numSensors; i++) {
        if (_sensors[i] &&
            _sensors[i]->getSensorState() == SensorStatus::LOST) {
            delete _sensors[i];
            _sensors[i] = nullptr;
        }
    }
}
