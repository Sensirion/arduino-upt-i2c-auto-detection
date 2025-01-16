#include "SensorList.h"

static const char* TAG = "SensorList";

size_t SensorList::hashSensorType(const SensorType sensorType) {
    SensorHash sensorHash = SensorHash::UNDEFINED;
    switch (sensorType) {
        case SensorType::SCD4X:
            sensorHash = SensorHash::SCD4X;
            break;
        case SensorType::SFA3X:
            sensorHash = SensorHash::SFA3X;
            break;
        case SensorType::SVM41:
        case SensorType::SVM4X:
            sensorHash = SensorHash::SVM4X;
            break;
        case SensorType::SHT4X:
            sensorHash = SensorHash::SHT4X;
            break;
        case SensorType::SAV45:
            sensorHash = SensorHash::UNDEFINED;  // SAV45 not yet supported
            break;
        case SensorType::SEN50:
        case SensorType::SEN54:
        case SensorType::SEN55:
        case SensorType::SEN5X:
            sensorHash = SensorHash::SEN5X;
            break;
        case SensorType::SCD30:
            sensorHash = SensorHash::SCD30;
            break;
        case SensorType::STC31:
        case SensorType::STC3X:
            sensorHash = SensorHash::STC3X;
            break;
        case SensorType::SGP41:
        case SensorType::SGP4X:
            sensorHash = SensorHash::SGP4X;
            break;
        case SensorType::SEN66:
            sensorHash = SensorHash::SEN66;
            break;
        default:
            sensorHash = SensorHash::UNDEFINED;
    }

    if (sensorHash == SensorHash::UNDEFINED) {
        ESP_LOGE(TAG, "Error: invalid sensor type %s encountered. Aborting!",
                 sensorLabel(sensorType));
        assert(0);
    }

    // Subtracting 1 to hash from 0 up to and including SensorManager's
    // _MAX_NUM_SENSORS
    return static_cast<size_t>(sensorHash) - 1;
}

SensorList::SensorList(const uint8_t numSensors) : mNumSensors(numSensors) {
    mSensors = new SensorStateMachine*[mNumSensors];
    for (size_t i = 0; i < mNumSensors; i++) {
        mSensors[i] = nullptr;
    }
}

SensorList::~SensorList() {
    delete[] mSensors;
}

void SensorList::addSensor(ISensor* pSensor) const {
    // Hash
    const size_t hashIndex = hashSensorType(pSensor->getSensorType());
    // Insert
    if (mSensors[hashIndex] == nullptr) {
        mSensors[hashIndex] = new SensorStateMachine(pSensor);
        return;
    }
}

size_t SensorList::count() const {
    size_t numberOfSensors = 0;
    for (int i = 0; i < mNumSensors; ++i) {
        if (mSensors[i]) {
            ++numberOfSensors;
        }
    }
    return numberOfSensors;
}

size_t SensorList::getTotalNumberOfDataPoints() const {
    size_t totalNumberOfDataPoints = 0;
    for (int i = 0; i < mNumSensors; ++i) {
        if (mSensors[i]) {
            totalNumberOfDataPoints +=
                mSensors[i]->getSensor()->getNumberOfDataPoints();
        }
    }
    return totalNumberOfDataPoints;
}

int SensorList::getLength() const {
    return mNumSensors;
}

SensorStateMachine* SensorList::getSensorStateMachine(const size_t i) const {
    return mSensors[i];
}

ISensor* SensorList::getSensor(SensorType sensorType) const {
    const size_t sensorHashIndex = hashSensorType(sensorType);
    if (mSensors[sensorHashIndex]) {
        return mSensors[sensorHashIndex]->getSensor();
    }
    return nullptr;
}

bool SensorList::containsSensor(SensorType sensorType) const {
    const size_t qHashIndex = hashSensorType(sensorType);

    // Check (1) if a sensor of this type is known and (2) if the version
    // matches as well
    if (mSensors[qHashIndex] &&
        mSensors[qHashIndex]->getSensor()->getSensorType() == sensorType) {
        return true;
    }

    return false;
}

void SensorList::removeLostSensors() const {
    for (size_t i = 0; i < mNumSensors; i++) {
        if (mSensors[i] &&
            mSensors[i]->getSensorState() == SensorStatus::LOST) {
            delete mSensors[i];
            mSensors[i] = nullptr;
        }
    }
}
