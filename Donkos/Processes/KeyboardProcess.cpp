#include "KeyboardProcess.h"
#include "DonkosInternal.h"

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
}

