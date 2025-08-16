#ifndef DONKOSINTERNAL_H
#define DONKOSINTERNAL_H

#include "Process.h"
#include "RoundRobinScheduler.h"

void Donkos_GenericProcessMain();

void Donkos_StartNewProcess(Process *process);

void Donkos_YieldProcess(Process *process);

void Donkos_BlockProcess(Process *process);

void Donkos_EndProcess(Process *process);

void Donkos_Display(int32_t line, const char* text);

void Donkos_RequestScheduling();

void Donkos_ContextSwitch(uint32_t *regArray);

void Donkos_Tick();

void Donkos_ServiceHandler(uint32_t svcNumber , Process * process);

#endif //DONKOSINTERNAL_H
