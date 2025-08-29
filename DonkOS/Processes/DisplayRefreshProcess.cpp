#include "DisplayRefreshProcess.h"
#include "DonkosInternal.h"

DisplayRefreshProcess::DisplayRefreshProcess(BaseDisplay *display) : display{display} {
}


void DisplayRefreshProcess::Main() {
    display->Init();

    while (true) {
        display->Refresh();
        wait(100);
    }
}

