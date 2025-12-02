#include "EPaperDisplayProcess.h"
#include "DonkosInternal.h"
#include "DonkosLogger.h"
#include "Drivers/epd/Examples/EPD_Test.h"


void EPaperDisplayProcess::Main() {
    EPD_test();
    while (true) {
        Donkos_YieldProcess(this);
    }
}
