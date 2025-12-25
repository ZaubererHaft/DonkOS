#ifndef DONKOS_BME68XPROCESS_H
#define DONKOS_BME68XPROCESS_H

#include "bme68x_defs.h"
#include "Process.h"

class BME68xProcess : public Process {
public:
    BME68xProcess();

    void Main() override;

private:
    bme68x_data data;
};


#endif //DONKOS_BME68XPROCESS_H
