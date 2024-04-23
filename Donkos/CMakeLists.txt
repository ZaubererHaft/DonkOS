add_library(Donko UserTask.cpp context_switch.S
        Process.cpp
        Processes/ProcessMutex.cpp
        Processes/ProcessLed1.cpp
        Processes/ProcessLed2.cpp
)
target_include_directories(Donko PUBLIC Inc Processes/Inc)
