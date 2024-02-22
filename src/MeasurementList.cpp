#include "MeasurementList.h"

void MeasurementList::init(const size_t& length) {
    _length = length;
    delete[] _measurements;
    _measurements = new Measurement[_length];
}

size_t MeasurementList::getLength() const {
    return _length;
}

MeasurementList::~MeasurementList() {
    delete[] _measurements;
}

MeasurementList::MeasurementList(const MeasurementList& other)
    : _length(other._length) {
    _measurements = new Measurement[_length];
    for (size_t i = 0; i < _length; i++) {
        _measurements[i] = other._measurements[i];
    }
}

MeasurementList& MeasurementList::operator=(const MeasurementList& other) {
    if (&other != this) {
        delete[] _measurements;

        _length = other._length;
        _measurements = new Measurement[_length];
        for (size_t i = 0; i < _length; i++) {
            _measurements[i] = other._measurements[i];
        }
        _writeHead = other._writeHead;
    }
    return *this;
}

void MeasurementList::addMeasurement(const Measurement& m) {
    if (_writeHead < _length) {
        _measurements[_writeHead] = m;
        _writeHead++;
    }
}

Measurement MeasurementList::getMeasurement(size_t i) const {
    return _measurements[i];
}

void MeasurementList::resetWriteHead() {
    _writeHead = 0;
}
