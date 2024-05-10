add_library(Donko Donkos.cpp
        Processes/Process.cpp
        Processes/Process7SegmentDisplay.cpp
        Processes/ProcessLed1.cpp
        Processes/ProcessLed2.cpp
        Scheduler/context_switch.S
        Scheduler/Scheduler.cpp
        svc_handler.S
        Processes/ProcessNoLoop.cpp
        Processes/Inc/ProcessNoLoop.h
)
target_include_directories(Donko PUBLIC Inc Processes/Inc Scheduler/Inc)
target_include_directories(Donko PRIVATE .)
