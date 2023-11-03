#include "utils.h"

bool timeIntervalPassed(const uint32_t interval,
                        const uint32_t currentTimeStamp,
                        const uint32_t latestUpdateTimeStamp) {
    if (currentTimeStamp < interval) {
        return true;
    }
    unsigned long elapsedTime = currentTimeStamp - latestUpdateTimeStamp;
    return elapsedTime >= interval;
}
