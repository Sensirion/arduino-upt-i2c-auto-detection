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
