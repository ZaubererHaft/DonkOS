#include "KeyboardProcess.h"
#include "DonkosInternal.h"
#include "main.h"

KeyboardProcess::KeyboardProcess() : show{false} {

}

void KeyboardProcess::Main() {
    show = !show;
    Donkos_SetDisplayLine(2);

    if(show)
    {
        Donkos_Display("Hi!");
    }
    else
    {
        Donkos_Display("            ");
    }

    wait(500);
}

void KeyboardProcess::SetKeyPressed(uint16_t pin) {
}

