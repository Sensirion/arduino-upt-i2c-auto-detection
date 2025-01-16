#ifndef MEASUREMENT_LIST_H
#define MEASUREMENT_LIST_H

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
    size_t mLength = 0;
    size_t mWriteHead = 0;
    Measurement* mMeasurements = nullptr;
};

#endif /* MEASUREMENT_LIST_H */
