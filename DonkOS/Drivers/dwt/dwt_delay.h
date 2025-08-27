#ifndef DWT_DELAY_H_
#define DWT_DELAY_H_

#include <cstdint>

void DWT_Init(void);
void DWT_Delay(uint32_t us);
uint32_t DWT_GetTick();

#endif
