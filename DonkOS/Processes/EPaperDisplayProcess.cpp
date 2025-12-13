#include "EPaperDisplayProcess.h"
#include "DonkosLogger.h"
#include "EPD_4in2_V2.h"
#include "Drivers/epd/Examples/EPD_Test.h"

namespace {
    constexpr UWORD Imagesize = ((EPD_4IN2_V2_WIDTH % 8 == 0) ? (EPD_4IN2_V2_WIDTH / 8) : (EPD_4IN2_V2_WIDTH / 8 + 1)) *
                                EPD_4IN2_V2_HEIGHT;
    UBYTE BlackImage[Imagesize];
}

void EPaperDisplayProcess::Main() {
    Logger_Debug("Init display...");
    if (DEV_Module_Init() == 0) {
    } else {
        Logger_Error("Display init failed!");
    }
    Logger_Debug("e-Paper Init and Clear...\r\n");


    EPD_4IN2_V2_Init();
    EPD_4IN2_V2_Clear();
    wait(500);

    Paint_NewImage(BlackImage, EPD_4IN2_V2_WIDTH, EPD_4IN2_V2_HEIGHT, 0, WHITE);
    Paint_Clear(WHITE);
    Paint_DrawRectangle(5, 5, 105, 105, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    EPD_4IN2_V2_Display(BlackImage);

    wait(2000);

    Paint_NewImage(BlackImage, 100, 100, 0, WHITE);
    Paint_Clear(WHITE);
    Paint_DrawRectangle(0, 0, 100, 100, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    EPD_4IN2_V2_PartialDisplay(BlackImage, 115, 5, 215, 105);

    wait(2000);
}
