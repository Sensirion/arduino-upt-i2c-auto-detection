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
#ifndef _I_SENSOR_H_
#define _I_SENSOR_H_

// #include "DataPoint.h"
#include "Sensirion_UPT_Core.h"
#include <Arduino.h>

class ISensor {
  public:
    virtual ~ISensor() = default;
    /**
     * @brief Initialize sensor driver such that data may be read out
     *
     * @return A uint16_t error corresponding to SensirionErrors.h of
     * SensirionCore, where 0 value corresponds to no error.
     */
    virtual uint16_t start() = 0;
    /**
     * @brief Call driver methods to perform measurement and update DataPoints
     *
     * @param dataPoints argument must be at least getNumberOfDataPoints()
     * long
     *
     * @param timeStamp at time of function call, represents milliseconds
     * passed since program startup.
     *
     * @return A uint16_t error corresponding to SensirionErrors.h of
     * SensirionCore, where 0 value corresponds to no error.
     */
    virtual uint16_t measureAndWrite(DataPoint dataPoints[],
                                     const unsigned long timeStamp) = 0;
    /**
     * @brief Get the specific SensorId of the ISensor realization
     *
     * @return SensorId
     */
    virtual const SensorID getSensorId() const = 0;
    /**
     * @brief Get the number of DataPoints this sensor occupies in the Data
     * object.
     *
     * @return size_t
     */
    virtual const size_t getNumberOfDataPoints() const = 0;
    /**
     * @brief Get the minimum measurement interval of the sensor. This must be
     * larger than the longest possible measurement duration.
     *
     * @return unsigned long
     */
    virtual const unsigned long getMinimumMeasurementInterval() const = 0;
    /**
     * @brief Get a pointer to the sensordriver, reinterpret_cast-ed into a void
     * pointer
     *
     * @return void*
     */
    virtual void* getDriver() = 0;
};

#endif /* _I_SENSOR_H_ */
