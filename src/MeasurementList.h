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
