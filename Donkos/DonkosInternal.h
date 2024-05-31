#ifndef TEST_DONKOSINTERNAL_H
#define TEST_DONKOSINTERNAL_H

#include "Process.h"

void Donkos_GenericProcessMain();

void Donkos_StartProcess(Process *process);

void Donkos_BlockProcess(Process *process);

void Donkos_EndProcess(Process *process);

void Donkos_DisplayNumber(uint8_t number);

void Donkos_DisableIRQ();

void Donkos_EnableIRQ();

#endif //TEST_DONKOSINTERNAL_H
