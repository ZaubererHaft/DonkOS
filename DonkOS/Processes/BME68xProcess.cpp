#include "BME68xProcess.h"

#include "bme68x_necessary_functions.h"
#include "main.h"

extern I2C_HandleTypeDef hi2c1;

BME68xProcess::BME68xProcess() : data{} {
}

void BME68xProcess::Main() {
    if (bme68x_start(&data, &hi2c1) == 0) {
        while (1) {
            if (bme68x_single_measure(&data) == 0) {
                // Measurement is successful, so continue with IAQ
                data.iaq_score = bme68x_iaq(); // Calculate IAQ

                wait(5000);
            }
        }
    }
}
