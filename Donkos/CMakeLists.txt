add_library(Donko Donkos.cpp
        Processes/Process.cpp
        Processes/Process7SegmentDisplay.cpp
        Processes/ProcessLed1.cpp
        Processes/ProcessLed2.cpp
        Scheduler/Scheduler.cpp
        Processes/ProcessNoLoop.cpp
        Processes/ProcessMatrixDisplay.cpp
        Processes/store_context.S
        Processes/load_context.S
        Processes/NTCTemperatureProcess.cpp
        Services/Display.cpp
        Processes/DHTProcess.cpp
        Processes/Inc/DHTProcess.h
)
target_include_directories(Donko PUBLIC Inc Processes/Inc Scheduler/Inc Services/Inc DataStructures/Inc)
target_include_directories(Donko PRIVATE .)
