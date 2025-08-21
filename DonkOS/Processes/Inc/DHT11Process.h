#ifndef DONKOS_DHT11PROCESS_H
#define DONKOS_DHT11PROCESS_H

#include "Process.h"
#include "main.h"

class DHT11Process : public Process
{
public:
    DHT11Process();

    void SetHandle(TIM_HandleTypeDef handle);

    void Main() override;

private:
    TIM_HandleTypeDef htim;

    void microDelay(uint16_t delay);

    static void setDataOutput();

    static void setDataInput();

    bool initCommunication();

    uint8_t Read();

    void EndCommunication();
};


#endif //DONKOS_DHT11PROCESS_H
