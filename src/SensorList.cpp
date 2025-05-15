#include "SensorList.h"
#include <algorithm>
#include <iterator>

static const char* TAG = "SensorList";


SensorList::~SensorList() {
    for( auto s : mSensorCollection){
        delete s;
    }
    mSensorCollection.clear();
}

void SensorList::addSensor(ISensor* pSensor) {
    // Hash

    auto found = std::find_if(mSensorCollection.begin(),
    mSensorCollection.end(), [pSensor](SensorStateMachine* x) {
        return x->getSensor()->getSensorType() == pSensor->getSensorType();
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

ISensor* SensorList::getSensor(SensorType sensorType) const {
    for( const auto s : mSensorCollection){
        if (s->getSensor()->getSensorType() == sensorType){
            return s->getSensor();
        }
    }
    return nullptr;
}

bool SensorList::containsSensor(SensorType sensorType) const {
    auto iter = std::find_if(mSensorCollection.begin(),
    mSensorCollection.end(), [sensorType](SensorStateMachine* s) {
        return s->getSensor()->getSensorType() == sensorType;
    });
    return iter != mSensorCollection.end();
}

void SensorList::removeLostSensors() {
    std::vector<SensorStateMachine*> livingSensors{};
    for (auto s: mSensorCollection){
        if(s->getSensorState() != SensorStatus::LOST){
            livingSensors.push_back(s);
        }
        else{
            delete s;
        }
    }
    mSensorCollection.clear();
    mSensorCollection = livingSensors;
}
