#include "MeasurementList.h"

void MeasurementList::init(const size_t& length) {
    mLength = length;
    delete[] mMeasurements;
    mMeasurements = new Measurement[mLength];
}

size_t MeasurementList::getLength() const {
    return mLength;
}

MeasurementList::~MeasurementList() {
    delete[] mMeasurements;
}

MeasurementList::MeasurementList(const MeasurementList& other)
    : mLength(other.mLength) {
    mMeasurements = new Measurement[mLength];
    for (size_t i = 0; i < mLength; i++) {
        mMeasurements[i] = other.mMeasurements[i];
    }
}

MeasurementList& MeasurementList::operator=(const MeasurementList& other) {
    if (&other != this) {
        delete[] mMeasurements;

        mLength = other.mLength;
        mMeasurements = new Measurement[mLength];
        for (size_t i = 0; i < mLength; i++) {
            mMeasurements[i] = other.mMeasurements[i];
        }
        mWriteHead = other.mWriteHead;
    }
    return *this;
}

void MeasurementList::addMeasurement(const Measurement& m) {
    if (mWriteHead < mLength) {
        mMeasurements[mWriteHead] = m;
        mWriteHead++;
    }
}

Measurement MeasurementList::getMeasurement(size_t i) const {
    return mMeasurements[i];
}

void MeasurementList::resetWriteHead() {
    mWriteHead = 0;
}
