#ifndef DONKOS_DHT11NONBLOCKINGPROCESS_H
#define DONKOS_DHT11NONBLOCKINGPROCESS_H

#include "Process.h"
#include "main.h"


class DHT11NonblockingProcess : public Process
{
public:
    DHT11NonblockingProcess();

    void SetHandle(TIM_HandleTypeDef handle);

    void Main() override;

    void TimerInterruptReceived();

private:
    enum class DHT_STATE
    {
        IDLE,
        COMM_INITIALIZED,
        READY_FOR_BIT,
        COMM_INITIALIZED_WAIT,
        COMM_INITIALIZED_WAIT_FOR_DEVICE,
        COMM_ERROR,
        WAIT_FOR_BIT,
        READ_BIT,
        COMM_END,
        PROCESS_DATA,
        RESTART
    };

    DHT_STATE state;
    TIM_HandleTypeDef htim;
};


#endif //DONKOS_DHT11NONBLOCKINGPROCESS_H
