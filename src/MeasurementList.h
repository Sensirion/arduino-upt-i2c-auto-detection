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

#ifndef _MEASUREMENT_LIST_H_
#define _MEASUREMENT_LIST_H_

#include "Sensirion_UPT_Core.h"

/* Class handling a collection of Measurements */
class MeasurementList {
  public:
    MeasurementList() = default;
    ~MeasurementList();
    MeasurementList(const MeasurementList& other);  // Copy Constructor
    MeasurementList&
    operator=(const MeasurementList& other);  // Assignment operator

    /**
     * @brief Allocates memory for requested number of Measurements
     *
     * @param length desired size
     */
    void init(const size_t& length);

    /**
     * @brief getter method for _length
     */
    size_t getLength() const;

    /**
     * @brief adds a Measurement
     *
     * @note Does not report a failure to perform the operation
     */
    void addMeasurement(const Measurement&);

    /**
     * @brief getter method for stored Measurements
     */
    Measurement getMeasurement(size_t) const;

    /**
     * @brief reset _writeHead to start of list
     */
    void resetWriteHead();

  private:
    size_t _length = 0;
    size_t _writeHead = 0;
    Measurement* _measurements = nullptr;
};

#endif /* _MEASUREMENT_LIST_H_ */
