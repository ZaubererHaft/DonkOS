//
// Created by ludwig on 11/12/25.
//

#ifndef DONKOS_GPSPROCESS_H
#define DONKOS_GPSPROCESS_H

#include "Process.h"
#include "main.h"
#include "BaseDisplay.h"
#include "NMEAParser.h"

class GPSProcess : public Process {
public:
    explicit GPSProcess(BaseDisplay *display);

    void Main() override;

    void SetHandle(const UART_HandleTypeDef &handle);

    void UartReceived(uint16_t size);

    bool RestartCommunication();

private:
    static constexpr int32_t line_size = 82;
    static constexpr int32_t buffer_size = line_size * 12;

    int32_t ReceivedSize = 0;
    bool restart = false;
    uint8_t buffer[buffer_size];
    char lineBuffer[line_size];
    NMEAParsedMessage messages_buffer[10];
    NMEAParser parser;
};


#endif //DONKOS_GPSPROCESS_H
