#include "BME68xProcess.h"

#include "bme68x_necessary_functions.h"
#include "DonkosInternal.h"
#include "DonkosLogger.h"
#include "main.h"
#include "StringConverter.h"

extern I2C_HandleTypeDef hi2c1;

BME68xProcess::BME68xProcess(SimpleLock *i2c_lock) : lock{i2c_lock}, data{} {
}

void BME68xProcess::Main() {
    Donkos_Display(5, 2, "Verbinde BME...");


    lock->AutoLock(this);
    auto result = bme68x_start(&data, &hi2c1) == 0;
    lock->Unlock(this);

    if (result) {
        while (true) {
            lock->AutoLock(this);
            result = bme68x_single_measure(&data) == 0;
            lock->Unlock(this);

            if (result) {
                StringConverter conv{};
                char test[20] = "T:       P: ";
                conv.FloatToString(data.temperature, &test[3], sizeof(test) - 3, {
                                       .string_termination = false
                                   });
                conv.FloatToString(data.pressure / 100000.0f, &test[12], sizeof(test) - 12);

                char humidity[] = "H:         ";
                auto [stat, index] = conv.FloatToString(data.humidity, &humidity[3], sizeof(humidity) - 3,
                                                        {
                                                            .string_termination = false
                                                        });
                humidity[index + 4] = '%';
                humidity[index + 5] = '\0';

                Donkos_Display(5, 2, test);
                Donkos_Display(5, 3, humidity);
            } else {
                Logger_Error("Failed to measure with BME688 :(");
            }
            wait(5000);
        }
    }
}
