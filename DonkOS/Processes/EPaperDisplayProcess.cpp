#include "EPaperDisplayProcess.h"
#include "DonkosLogger.h"
#include "EPD_4in2_V2.h"
#include "GUI_Paint.h"
#include "epd_image.h"

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
    Logger_Debug("e-Paper Init and Clear...");
    EPD_4IN2_V2_Init();
    EPD_4IN2_V2_Clear();
    wait(500);

    Logger_Debug("Draw Data...");



    Paint_NewImage(BlackImage, EPD_4IN2_V2_WIDTH, EPD_4IN2_V2_HEIGHT, 0, WHITE);
    Paint_Clear(WHITE);

    Paint_DrawBitMap(epd_image);

    Paint_DrawString_EN(5, 5, "Outdoor", &Font20, WHITE, BLACK);
    Paint_DrawString_EN(140, 5, "Forecast", &Font20, WHITE, BLACK);
    Paint_DrawString_EN(275, 5, "Indoor", &Font20, WHITE, BLACK);

    Paint_DrawString_EN(5, 35, "3.50 C", &Font24, WHITE, BLACK);
    Paint_DrawString_EN(5, 65, "80%", &Font24, WHITE, BLACK);

    Paint_DrawString_EN(140, 35, "10.20 C", &Font24, WHITE, BLACK);
    Paint_DrawString_EN(140, 65, "60%", &Font24, WHITE, BLACK);

    Paint_DrawString_EN(275, 35, "18.10 C", &Font24, WHITE, BLACK);
    Paint_DrawString_EN(275, 65, "55%", &Font24, WHITE, BLACK);

    //Paint_DrawRectangle(5, 5, 105, 105, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    EPD_4IN2_V2_Display(BlackImage);

    wait(2000);

  //  Paint_NewImage(BlackImage, 100, 100, 0, WHITE);
  //  Paint_Clear(WHITE);
  //  Paint_DrawRectangle(0, 0, 100, 100, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  //  EPD_4IN2_V2_PartialDisplay(BlackImage, 115, 5, 215, 105);
//
  //  wait(2000);
}
