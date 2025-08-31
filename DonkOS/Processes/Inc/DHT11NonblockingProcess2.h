#ifndef DONKOS_DHT11NONBLOCKINGPROCESS2_H
#define DONKOS_DHT11NONBLOCKINGPROCESS2_H


#include "Process.h"

class DHT11NonblockingProcess2 : public Process {
public:
    DHT11NonblockingProcess2();

    void Main() override;

    void InterruptReceived();

private:
    enum class DHT_STATE {
        IDLE,
        COMM_INITIALIZED,
        READY_FOR_BIT,
        COMM_INITIALIZED_WAIT,
        COMM_INITIALIZED_WAIT_FOR_DEVICE,
        COMM_INITIALIZED_WAIT_FOR_DEVICE_HIGH,
        COMM_INITIALIZED_WAIT_FOR_DEVICE_LOW,
        COMM_ERROR,
        WAIT_FOR_BIT,
        READ_BIT,
        PROCESS_DATA,
        RESTART,
        COMM_END
    };

    DHT_STATE state;
    uint8_t data_received[5];
    uint32_t bits_received;
    uint32_t cycles;
    uint32_t ticks_last_active_irq;
    bool clearDisplay;

    void restart_state();

    void idle_state();

    void process_data_state();

    void comm_error_state();
};


#endif //DONKOS_DHT11NONBLOCKINGPROCESS2_H
