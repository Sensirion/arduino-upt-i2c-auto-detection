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
#include "Data.h"

void Data::init(const size_t& length) {
    _length = length;
    delete[] _dataPoints;
    _dataPoints = new DataPoint[_length];
}

size_t Data::getLength() const {
    return _length;
}

Data::~Data() {
    delete[] _dataPoints;
}

Data::Data(Data&& src) : _dataPoints(src._dataPoints), _length(src._length) {
    delete[] src._dataPoints;
    src._length = 0;
}

Data& Data::operator=(Data&& src) {
    if (&src != this) {
        delete[] _dataPoints;

        this->_dataPoints = src._dataPoints;
        src._dataPoints = nullptr;
        this->_length = src._length;
        src._length = 0;
    }
    return *this;
}

void Data::addDataPoint(const DataPoint& dp) {
    if (_writeHead < _length - 1) {
        _dataPoints[_writeHead] = dp;
        _writeHead++;
    }
}

const DataPoint& Data::getDataPoint(size_t i) const {
    return _dataPoints[i];
}
