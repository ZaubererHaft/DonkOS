add_library(Donko Donkos.cpp
        Processes/Process.cpp
        Processes/ProcessMutex.cpp
        Processes/ProcessLed1.cpp
        Processes/ProcessLed2.cpp
        Scheduler/context_switch.S
        Scheduler/Scheduler.cpp
)
target_include_directories(Donko PUBLIC Inc Processes/Inc Scheduler/Inc)
target_include_directories(Donko PRIVATE .)
