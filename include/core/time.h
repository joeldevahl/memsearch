#ifndef CORE_TIME_H
#define CORE_TIME_H

#include <core/types.h>

#ifdef __cplusplus
extern "C" {
#endif

uint64_t time_current();
uint64_t time_frequency();

#ifdef __cplusplus
}
#endif

#endif // CORE_TIME_H
