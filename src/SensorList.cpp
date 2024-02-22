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

void SensorList::addSensor(ISensor* pSensor) {
    // Check if we already have it
    SensorType newSensorType = pSensor->getSensorType();
    if (containsSensor(newSensorType)) {
        return;
    }
    // Hash
    size_t hashIndex = _hashSensorType(newSensorType);
    // Insert
    if (_sensors[hashIndex] == nullptr) {
        _sensors[hashIndex] = new SensorStateMachine(pSensor);
        return;
    }

    return;
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
