set(Donkos_Sources
        Donkos.cpp

        Algorithms/StringConverter.cpp
        Algorithms/StringConverter.cpp

        Scheduler/RoundRobinScheduler.cpp
        Scheduler/PriortityScheduler.cpp

        #Services/Display.cpp
        Services/LedDisplay.cpp
        Services/NTCSensor.cpp
        Services/Lock.cpp
        Services/Latch.cpp

        Drivers/SSD1306/fonts.cpp
        Drivers/SSD1306/ssd1306.cpp
        Drivers/dwt/dwt_delay.cpp;
        Drivers/nmea/NMEParser.cpp

        Drivers/epd/e-Paper/EPD_4in2_V2.cpp
        Drivers/epd/Examples/EPD_4in2_V2_test.cpp
        Drivers/epd/GUI/GUI_Paint.cpp
        Drivers/epd/Fonts/font8.cpp
        Drivers/epd/Fonts/font12.cpp
        Drivers/epd/Fonts/font16.cpp
        Drivers/epd/Fonts/font20.cpp
        Drivers/epd/Fonts/font24.cpp
        Drivers/epd/Examples/ImageData.cpp
        Drivers/epd/Config/DEV_Config.cpp

        DataStructures/MemoryBlock.cpp
        DataStructures/MemoryListAllocator.cpp

        Processes/Process.cpp
        Processes/Process7SegmentDisplay.cpp
        Processes/ProcessLed.cpp
        #Processes/BuzzerProcess.cpp
        #Processes/ProcessMatrixDisplay.cpp
        Processes/store_context.S
        Processes/load_context.S
        #Processes/DACProcess.cpp
        Processes/ADC3Process.cpp
        Processes/DHT11Process.cpp
        Processes/DHT11NonblockingProcess.cpp
        Processes/DHT11NonblockingProcess2.cpp
        Processes/DisplayRefreshProcess.cpp
        #Processes/KeyboardProcess.cpp
        Processes/DiagramPageProcess.cpp
        Processes/GPSProcess.cpp
        Processes/WiFiProcess.cpp
        Processes/EPaperDisplayProcess.cpp
)

add_library(DonkOSLib ${Donkos_Sources})

target_include_directories(DonkOSLib PUBLIC
        Inc
        Processes/Inc
        Scheduler/Inc
        Services/Inc
        DataStructures/Inc
        Drivers/SSD1306/Inc
        Drivers/dwt
        Drivers/epd/e-Paper
        Drivers/epd/Config
        Drivers/epd/Examples
        Drivers/epd/GUI
        Drivers/nmea
        Algorithms/Inc
)

target_include_directories(DonkOSLib PRIVATE
        .
)

target_link_libraries(DonkOSLib stm32cubemx)

target_compile_definitions(DonkOSLib PUBLIC
        -D${CMAKE_BUILD_TYPE}
)