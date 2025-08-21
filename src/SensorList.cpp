#include "SensorList.h"
#include <algorithm>
#include <iterator>

static const char* TAG = "SensorList";


SensorList::~SensorList() {}

void SensorList::addSensor(ISensor* pSensor) {
    auto found = std::find_if(mSensorCollection.begin(),
    mSensorCollection.end(), [pSensor](SensorStateMachine* x) {
        return x->getSensor()->getDeviceType() == pSensor->getDeviceType();
    });
    if (found == mSensorCollection.end()){
        mSensorCollection.push_back(new SensorStateMachine(pSensor));
    }
}


size_t SensorList::getTotalNumberOfDataPoints() const {
    size_t totalNumberOfDataPoints = 0;
    for (auto s : mSensorCollection) {
        totalNumberOfDataPoints +=
                s->getSensor()->getNumberOfDataPoints();
    }
    return totalNumberOfDataPoints;
}


SensorStateMachine* SensorList::getSensorStateMachine(const size_t i) const {
    if (i >= mSensorCollection.size()){
        return nullptr;
    }
    return mSensorCollection[i];
}

ISensor* SensorList::getSensor(sensirion::upt::core::DeviceType deviceType) const {
    for( const auto s : mSensorCollection){
        if (s->getSensor()->getDeviceType() == deviceType){
            return s->getSensor();
        }
    }
    return nullptr;
}

bool SensorList::containsSensor(sensirion::upt::core::DeviceType deviceType) const {
    auto iter = std::find_if(mSensorCollection.begin(),
    mSensorCollection.end(), [deviceType](SensorStateMachine* s) {
        return s->getSensor()->getDeviceType() == deviceType;
    });
    return iter != mSensorCollection.end();
}

void SensorList::removeLostSensors() {
    std::vector<SensorStateMachine*> livingSensors{};
    for (auto s: mSensorCollection){
        if(s->getSensorState() != SensorStatus::LOST){
            livingSensors.push_back(s);
        }
    }
    mSensorCollection.clear();
    mSensorCollection = livingSensors;
}
