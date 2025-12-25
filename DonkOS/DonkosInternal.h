#ifndef DONKOSINTERNAL_H
#define DONKOSINTERNAL_H

#include "Process.h"
#include "RoundRobinScheduler.h"

enum class ServiceCall : int32_t
{
    UNREGISTER_RESCHEDULE,
    RESCHEDULE,
    START_PROCESS,
    MARKER_END
};

enum class Connectivity {
    NO_CONNECTION,
    WIFI_CONNECTED_NO_GPS,
    GPS_CONNECTED_NO_WIFI,
    GPS_AND_WIFI_CONNECTED,
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

void Donkos_ExternalInterruptReceived(int32_t id);

int32_t Donkos_GetSystemState();

void Donkos_TimerElapsed(int32_t timerId);

void Donkos_SleepCurrentProcess(int32_t ms);

void Donkos_YieldCurrentProcess();


#endif //DONKOSINTERNAL_H
