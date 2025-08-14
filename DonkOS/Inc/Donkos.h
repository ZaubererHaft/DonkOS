#ifndef DONKOS_H
#define DONKOS_H

#include "BaseScheduler.h"

void Donkos_Main();

void Donkos_RequestScheduling();

void Donkos_ContextSwitch(uint32_t *regArray);

void Donkos_Tick();

void Donkos_ServiceHandler(uint32_t *args);

#endif //DONKOS_H
