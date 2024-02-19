#include <stddef.h>
#include <sys/time.h>

#include "timestamp.h"

size_t getTimestamp() {
    struct timeval now;
    gettimeofday(&now, NULL);
    return now.tv_usec + (size_t) now.tv_sec * 1000000;
}
