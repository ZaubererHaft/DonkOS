#include "DisplayRefreshProcess.h"
#include "DonkosInternal.h"
#include "DonkosLogger.h"

DisplayRefreshProcess::DisplayRefreshProcess(BaseDisplay *display) : display{display} {
}


void DisplayRefreshProcess::Main() {
    if (!display->Init()) {
        Logger_Error("Failed to initialize display");
    } else {
        Logger_Debug("Display initialized!");
        while (true) {
            display->Refresh();
            wait(100);
        }
    }
}

