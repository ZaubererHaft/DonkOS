#ifndef LOGGER_H
#define LOGGER_H

#include <cstdio>

#ifdef DEBUG
#define Logger_Debug(__info, ...) printf("[DBG] " __info "\r\n" ,##__VA_ARGS__)
#else
#define Logger_Debug(...)
#endif

#ifdef DEBUG
#define Logger_Error(__info, ...) printf("[ERR] " __info "\r\n",##__VA_ARGS__)
#else
#define Logger_Error(...)
#endif


#ifdef DEBUG
#define Logger_Warning(__info, ...) printf("[WRN] " __info "\r\n",##__VA_ARGS__)
#else
#define Logger_Warning(...)
#endif

#endif