//
// Created by ludwig on 11/12/25.
//

#ifndef DONKOS_GPSPROCESS_H
#define DONKOS_GPSPROCESS_H

#include "Process.h"
#include "main.h"
#include "BaseDisplay.h"

class GPSProcess : public Process {
public:
    explicit GPSProcess(BaseDisplay *display);

    void Main() override;

    void SetHandle(const UART_HandleTypeDef &handle);

private:
    UART_HandleTypeDef huart_handle;

};


#endif //DONKOS_GPSPROCESS_H
