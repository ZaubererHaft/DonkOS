add_library(Donkos
        Donkos.cpp

        Algorithms/StringConverter.cpp
        Algorithms/StringConverter.cpp

        Scheduler/RoundRobinScheduler.cpp
        Scheduler/PriortityScheduler.cpp

        #Services/Display.cpp
        #Services/LedDisplay.cpp
        Services/NTCSensor.cpp

        #Drivers/SSD1306/fonts.cpp
        #Drivers/SSD1306/ssd1306.cpp

        DataStructures/MemoryBlock.cpp
        DataStructures/MemoryListAllocator.cpp

        Processes/Process.cpp
        Processes/Process7SegmentDisplay.cpp
        Processes/ProcessLed1.cpp
        Processes/ProcessLed2.cpp
        #Processes/BuzzerProcess.cpp
        #Processes/ProcessMatrixDisplay.cpp
        Processes/store_context.S
        Processes/load_context.S
        #Processes/DACProcess.cpp
        #Processes/ADC3Process.cpp
        #Processes/DHTProcess.cpp
        #Processes/SSD1306Process.cpp
        #Processes/KeyboardProcess.cpp
)

target_include_directories(Donkos PUBLIC
        Inc
        Processes/Inc
        Scheduler/Inc Services/Inc
        DataStructures/Inc
        Drivers/SSD1306/Inc
        Algorithms/Inc
)

target_include_directories(Donkos PRIVATE
        .
)

target_link_libraries(Donkos stm32cubemx)