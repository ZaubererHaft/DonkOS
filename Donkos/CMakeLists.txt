add_library(Donko UserTask.cpp context_switch.S
        Process.cpp
        Process.h
)
target_include_directories(Donko PUBLIC Inc)
