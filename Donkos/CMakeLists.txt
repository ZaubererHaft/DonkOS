add_library(Donko Donkos.cpp context_switch.S
        Process.cpp
        Processes/ProcessMutex.cpp
        Processes/ProcessLed1.cpp
        Processes/ProcessLed2.cpp
        Scheduler/Scheduler.cpp
        Scheduler/Inc/Scheduler.h
        Inc/Donkos.h
        Inc/DonkosInternal.h
        Inc/DonkosSettings.h
)
target_include_directories(Donko PUBLIC Inc Processes/Inc Scheduler/Inc)
