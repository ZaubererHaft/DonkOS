add_library(Donko Donkos.cpp
        Algorithms/StringConverter.cpp
        Processes/Process.cpp
        Processes/Process7SegmentDisplay.cpp
        Processes/ProcessLed1.cpp
        Processes/ProcessLed2.cpp
        Scheduler/RoundRobinScheduler.cpp
        Processes/ProcessMatrixDisplay.cpp
        Processes/store_context.S
        Processes/load_context.S
        Processes/ADC3Process.cpp
        Services/Display.cpp
        Services/LedDisplay.cpp
        Services/NTCSensor.cpp
        Processes/DHTProcess.cpp
        Processes/SSD1306Process.cpp
        Drivers/SSD1306/fonts.cpp
        Drivers/SSD1306/ssd1306.cpp
        Processes/BuzzerProcess.cpp
        DataStructures/MemoryBlock.cpp
        DataStructures/MemoryListAllocator.cpp
        Scheduler/Inc/BaseScheduler.h
        Algorithms/StringConverter.cpp
        Algorithms/Inc/StringConverter.h
        Processes/KeyboardProcess.cpp
        Processes/Inc/KeyboardProcess.h
        Scheduler/PriortityScheduler.cpp
        Scheduler/Inc/PriorityScheduler.h
)
target_include_directories(Donko PUBLIC Inc Processes/Inc Scheduler/Inc Services/Inc DataStructures/Inc  Drivers/SSD1306/Inc Algorithms/Inc)
target_include_directories(Donko PRIVATE .)
