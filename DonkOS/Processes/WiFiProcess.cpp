#include "Inc/WiFiProcess.h"

#include "../DonkosInternal.h"


WiFiProcess::WiFiProcess() {
}

void WiFiProcess::Main() {
    while (true) {
        Donkos_YieldProcess(this);
    }
}
