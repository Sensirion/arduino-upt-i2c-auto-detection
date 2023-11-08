#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>

/**
 * @brief verify if a given timespan has passed
 *
 * @param[in] interval duration of the timespan
 *
 * @param[in] currentTimeStamp current time
 *
 * @param[in] latestUpdateTimeStamp time at which the last update was
 * performed
 *
 * @param[out] boolean true if more time than interval has passed since
 * latestUpdateTimeStamp
 */
bool timeIntervalPassed(const uint32_t, const uint32_t, const uint32_t);

#endif /* _UTILS_H_ */