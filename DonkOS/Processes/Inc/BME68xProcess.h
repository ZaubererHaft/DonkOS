#ifndef DONKOS_BME68XPROCESS_H
#define DONKOS_BME68XPROCESS_H

#include "bme68x_defs.h"
#include "Lock.h"
#include "Process.h"

class BME68xProcess : public Process {
public:
    BME68xProcess(SimpleLock *i2c_lock);

    void Main() override;

private:
    SimpleLock *lock;
    bme68x_data data;
};


#endif //DONKOS_BME68XPROCESS_H
