add_library(Donko UserTask.cpp context_switch.S
        Process.cpp
        Processes/ProcessMutex.cpp
        Processes/ProcessLed1.cpp
        Processes/ProcessLed2.cpp
        Scheduler/Scheduler.cpp
        Scheduler/Inc/Scheduler.h
        Inc/System.h
)
target_include_directories(Donko PUBLIC Inc Processes/Inc Scheduler/Inc)
