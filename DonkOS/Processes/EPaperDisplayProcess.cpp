#include "EPaperDisplayProcess.h"
#include "DonkosInternal.h"
#include "DonkosLogger.h"
#include "Drivers/epd/Examples/EPD_Test.h"


void EPaperDisplayProcess::Main() {
   // Logger_Debug("Init display...")
   // if(DEV_Module_Init() == 0){
//
   // }
   // else {
   //     Logger_Error("Display init failed!");
   // }
//
   // printf("e-Paper Init and Clear...\r\n");
   // EPD_4IN2_V2_Init();
   // EPD_4IN2_V2_Clear();
   // DEV_Delay_ms(500);
//
   // //Create a new image cache
   // UBYTE *BlackImage;
   // /* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */
   // UWORD Imagesize = ((EPD_4IN2_V2_WIDTH % 8 == 0)? (EPD_4IN2_V2_WIDTH / 8 ): (EPD_4IN2_V2_WIDTH / 8 + 1)) * EPD_4IN2_V2_HEIGHT;
   // if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
   //     printf("Failed to apply for black memory...\r\n");
   //     return -1;
   // }
   // printf("Paint_NewImage\r\n");
   // Paint_NewImage(BlackImage, EPD_4IN2_V2_WIDTH, EPD_4IN2_V2_HEIGHT, 0, WHITE);
}
