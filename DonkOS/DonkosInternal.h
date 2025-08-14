#ifndef DONKOSINTERNAL_H
#define DONKOSINTERNAL_H

#include "Process.h"
#include "RoundRobinScheduler.h"

void Donkos_GenericProcessMain();

void Donkos_StartNewProcess(Process *process);

void Donkos_YieldProcess(Process *process);

void Donkos_BlockProcess(Process *process);

void Donkos_EndProcess(Process *process);

void Donkos_Display(const char* text);

void Donkos_SetDisplayLine(uint32_t line);

void Donkos_DisableIRQ();

void Donkos_EnableIRQ();

BaseScheduler *Donkos_GetScheduler();

#endif //DONKOSINTERNAL_H
