#ifndef TEST_DONKOSINTERNAL_H
#define TEST_DONKOSINTERNAL_H

#include "Process.h"

void Donkos_GenericProcessMain();

void Donkos_BlockProcess(Process *process);

void Donkos_EndProcess(Process *process);

void Donkos_DisplayNumber(uint8_t number);

#endif //TEST_DONKOSINTERNAL_H
