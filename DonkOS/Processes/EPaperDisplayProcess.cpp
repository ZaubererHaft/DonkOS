#include "EPaperDisplayProcess.h"
#include "DonkosInternal.h"
#include "DonkosLogger.h"

#include "epd4in2.h"
#include "epdif.h"
#include "epdpaint.h"
#include "imagedata.h"

#define COLORED      1
#define UNCOLORED    0

uint8_t frame_buffer[EPD_WIDTH * EPD_HEIGHT / 8];

void EPaperDisplayProcess::Main() {

    EPD epd{};
    if (EPD_Init(&epd) != 0) {
        Logger_Debug("e-Paper init failed\n");
        return;
    }

    Paint paint;
    Paint_Init(&paint, frame_buffer, epd.width, epd.height);
    Paint_Clear(&paint, UNCOLORED);

    /* Draw something to the frame_buffer */
    /* For simplicity, the arguments are explicit numerical coordinates */
    Paint_DrawRectangle(&paint, 20, 80, 180, 280, COLORED);
    Paint_DrawLine(&paint, 20, 80, 180, 280, COLORED);
    Paint_DrawLine(&paint, 180, 80, 20, 280, COLORED);
    Paint_DrawFilledRectangle(&paint, 200, 80, 360, 280, COLORED);
    Paint_DrawCircle(&paint, 300, 160, 60, UNCOLORED);
    Paint_DrawFilledCircle(&paint, 90, 210, 30, COLORED);

    /*Write strings to the buffer */
    Paint_DrawFilledRectangle(&paint, 0, 6, 400, 30, COLORED);
    Paint_DrawStringAt(&paint, 100, 10, "Hello world!", &Font24, UNCOLORED);
    Paint_DrawStringAt(&paint, 100, 40, "e-Paper Demo", &Font24, COLORED);

    /* Display the frame_buffer */
    EPD_DisplayFrame(&epd, frame_buffer);

    /* Display the image buffer */
    EPD_DisplayFrame(&epd, IMAGE_BUTTERFLY);

    while (true) {
        Donkos_YieldProcess(this);
    }
}
