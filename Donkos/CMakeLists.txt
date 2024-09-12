add_library(Donko Donkos.cpp
        Processes/Process.cpp
        Processes/Process7SegmentDisplay.cpp
        Processes/ProcessLed1.cpp
        Processes/ProcessLed2.cpp
        Scheduler/Scheduler.cpp
        Processes/ProcessMatrixDisplay.cpp
        Processes/store_context.S
        Processes/load_context.S
        Processes/ADC3Process.cpp
        Services/Display.cpp
        Services/LedDisplay.cpp
        Services/NTCSensor.cpp
        Processes/DHTProcess.cpp
        Processes/Inc/DHTProcess.h
        Processes/SSD1306Process.cpp
        Processes/Inc/SSD1306Process.h
        Drivers/SSD1306/fonts.cpp
        Drivers/SSD1306/ssd1306.cpp
        Drivers/SSD1306/Inc/fonts.h
        Drivers/SSD1306/Inc/ssd1306.h
        Processes/BuzzerProcess.cpp
        Processes/Inc/BuzzerProcess.h
)
target_include_directories(Donko PUBLIC Inc Processes/Inc Scheduler/Inc Services/Inc DataStructures/Inc  Drivers/SSD1306/Inc)
target_include_directories(Donko PRIVATE .)
