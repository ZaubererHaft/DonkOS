#ifndef LOGGER_H
#define LOGGER_H

#include <cstdio>

#ifdef DEBUG
#define Logger_Trace(...) printf(__VA_ARGS__)
#else
#define Logger_Trace(...)
#endif


#ifdef DEBUG
#define Logger_Debug(...) printf(__VA_ARGS__)
#else
#define Logger_Debug(...)
#endif

#endif