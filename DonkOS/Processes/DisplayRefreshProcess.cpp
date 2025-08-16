#include "DisplayRefreshProcess.h"
#include "DonkosInternal.h"

DisplayRefreshProcess::DisplayRefreshProcess(BaseDisplay *display) : display{display} {
}


void DisplayRefreshProcess::Main() {
    display->Init();

    while (true) {
        if (display->Dirty()) {
            display->Refresh();
            Donkos_YieldProcess(this);
        }
    }
}

