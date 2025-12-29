#include "SensorList.h"
#include <algorithm>
#include <iterator>

namespace sensirion::upt::i2c_autodetect{

constexpr auto TAG = "SensorList";


SensorList::~SensorList() {}


void SensorList::addSensorIfNotPresent(ISensor* pSensor) {
    if (!containsSensor(pSensor->getDeviceType())){
        mSensorCollection.push_back(new SensorStateMachine(pSensor));
    }
}


bool SensorList::containsSensor(uint8_t address) const{
        auto iter = std::find_if(mSensorCollection.begin(),
    mSensorCollection.end(), [address](SensorStateMachine* s) {
        return s->getSensor()->getI2CAddress() == address;
    });
    return iter != mSensorCollection.end();
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

ISensor* SensorList::getSensor(core::DeviceType deviceType) const {
    for( const auto s : mSensorCollection){
        if (s->getSensor()->getDeviceType() == deviceType){
            return s->getSensor();
        }
    }
    return nullptr;
}

bool SensorList::containsSensor(core::DeviceType deviceType) const {
    auto iter = std::find_if(mSensorCollection.begin(),
    mSensorCollection.end(), [deviceType](SensorStateMachine* s) {
        return s->getSensor()->getDeviceType() == deviceType;
    });
    return iter != mSensorCollection.end();
}

void SensorList::removeLostSensors() {
    std::vector<SensorStateMachine*> livingSensors{};
    std::vector<SensorStateMachine*> lostSensors{};
    for (auto s: mSensorCollection){
        if(s->getSensorState() != SensorStatus::LOST){
            livingSensors.push_back(s);
        }
        else{
            lostSensors.push_back(s);
        }
    }
    mSensorCollection.clear();
    mSensorCollection = livingSensors;
    for (auto s: lostSensors){
        delete s;
    }
}
} // namespace sensirion::upt::i2c_autodetect 
