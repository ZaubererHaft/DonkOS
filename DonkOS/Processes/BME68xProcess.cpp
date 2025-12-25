#include "BME68xProcess.h"

#include "bme68x_necessary_functions.h"
#include "DonkosLogger.h"
#include "main.h"
#include "StringConverter.h"

extern I2C_HandleTypeDef hi2c1;

BME68xProcess::BME68xProcess(SimpleLock *i2c_lock) : lock{i2c_lock}, data{} {
}

void BME68xProcess::Main() {
    lock->YieldLock(lock_id);
    auto result = bme68x_start(&data, &hi2c1) == 0;
    lock->Unlock(lock_id);

    if (result) {
        while (1) {
            lock->YieldLock(lock_id);
            result = bme68x_single_measure(&data) == 0;
            lock->Unlock(lock_id);

            if (result) {
                // Measurement is successful, so continue with IAQ
                char test[8];
                StringConverter{}.FloatToString(data.temperature, test, sizeof(test));
                char humidity[8];
                StringConverter{}.FloatToString(data.humidity, humidity, sizeof(humidity));

                Logger_Debug("BME688 Temperature is %s, Humidity %s", test, humidity);
            } else {
                Logger_Error("Failed to measure with BME688 :(");
            }
            wait(5000);
        }
    }
}
