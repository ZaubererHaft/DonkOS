#ifndef DONKOS_ATHAL_H
#define DONKOS_ATHAL_H

#include <cstdint>

void ATHAL_EnableChip();

void ATHAL_DisableChip();

bool ATHAL_StartUARTReceiveIT(uint8_t *data);

void ATHAL_Wait(int32_t ms);

bool ATHAL_Transmit(const uint8_t * ptr, int32_t length);

#endif //DONKOS_ATHAL_H