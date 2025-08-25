#ifndef DONKOSINTERNAL_H
#define DONKOSINTERNAL_H

#include "Process.h"
#include "RoundRobinScheduler.h"

enum class ServiceCall : int32_t
{
    UNREGISTER_RESCHEDULE = 0,
    RESCHEDULE = 1,
    START_PROCESS = 2
};

void Donkos_GenericProcessMain();

void Donkos_StartNewProcess(Process *process);

void Donkos_YieldProcess(Process *process);

void Donkos_BlockProcess(Process *process);

void Donkos_EndProcess(Process *process);

void Donkos_Display(int32_t page, int32_t line, const char* text);

void Donkos_ClearDisplay();

void Donkos_RequestScheduling();

void Donkos_ContextSwitch(uint32_t *regArray);

void Donkos_Tick();

void Donkos_ServiceHandler(ServiceCall svcNumber , Process * process);

void Donkos_KeyPressed(int32_t keyId);

int32_t Donkos_GetSystemState();

void Donkos_TimerElapsed(int32_t timerId);

#endif //DONKOSINTERNAL_H
