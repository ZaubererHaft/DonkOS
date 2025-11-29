#ifndef DONKOS_WIFIPROCESS_H
#define DONKOS_WIFIPROCESS_H

#include "Process.h"
#include "main.h"

extern UART_HandleTypeDef huart5;

class WiFiProcess : public Process{

public:
    WiFiProcess();

    void Main() override;


};


#endif //DONKOS_WIFIPROCESS_H