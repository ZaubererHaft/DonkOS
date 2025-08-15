#include "Donkos.h"
#include "Process7SegmentDisplay.h"
#include "ProcessLed1.h"
#include "ProcessLed2.h"
#include "RoundRobinScheduler.h"
#include "ADC3Process.h"

namespace {

    Process7SegmentDisplay mutexProcess{};
    ProcessLed1 led1Process{};
    ProcessLed2 led2Process{};
    ADC3Process adcProcess{};

    RoundRobinScheduler scheduler{};
}

extern ADC_HandleTypeDef hadc3;

void Donkos_Main() {
    SCB->CCR |= SCB_CCR_STKALIGN_Msk;

    scheduler.RegisterProcess(&mutexProcess);
    scheduler.RegisterProcess(&led1Process);
    scheduler.RegisterProcess(&led2Process);
    scheduler.RegisterProcess(&adcProcess);
    adcProcess.SetHandle(hadc3);

    scheduler.SetInitialProcess(&mutexProcess);

    NVIC_SetPriority(PendSV_IRQn, 0xFF);

    __set_CONTROL(0x3); //unprivileged mode starting here
    __ISB();
    __WFI();

    while (true) {
        __NOP();
    }
}

BaseScheduler *Donkos_GetScheduler() {
    return &scheduler;
}

void Donkos_RequestScheduling() {
    scheduler.Schedule();
    if (scheduler.NeedsContextSwitch()) {
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    }
}

void Donkos_StartNewProcess(Process *process) {
    asm("SVC #0x2;");
    //wait until task is registered to the scheduler
    //with that we should make sure that the argument on the stack is still valid
    //but not 100 % sure yet
    while (process->GetState() == ProcessState::CREATED) {
        __NOP();
    }
}

void Donkos_ContextSwitch(uint32_t *regArray) {
    scheduler.ContextSwitch(regArray);
}

void Donkos_Tick() {
    scheduler.Tick();
}

void Donkos_YieldProcess(Process *process) {
    //reschedule only. Here, a process can manually request a context switch
    asm("SVC #0x1;");
}

void Donkos_BlockProcess(Process *process) {
    asm("SVC #0x1;\n");

    // now wait until interrupt has been served
    // process will not be scheduled again until the resource the process was waiting for was freed
    // it will then continue executing after the while loop
    while (process->GetState() == ProcessState::WAITING) {
        __NOP();
    }
}

void Donkos_EndProcess(Process *process) {
    process->SetState(ProcessState::ENDED);
    //unregister and request scheduling again
    asm("SVC #0x0;");
    // wait for process to end
    while (true) {
        __NOP();
    }
}


void Donkos_Display(const char *text) {
    //ledDisplay.Display(text);
}

void Donkos_SetDisplayLine(uint32_t line) {
    // ledDisplay.SetLine(line);
}


void Donkos_GenericProcessMain() {
    Process *p = scheduler.GetCurrentProcess();
    p->Main();
    Donkos_EndProcess(p);
}


void Donkos_ServiceHandler(uint32_t svcNumber, Process *process) {

    //unregister & reschedule
    if (svcNumber == 0) {
        //since PENDSV has a lower prio than SVC it would safe to unregister without IRQ disabling
        // however since scheduling request is privileged -> SVC call
        scheduler.UnregisterProcess(process);
        Donkos_RequestScheduling();
    }
        //reschedule only
    else if (svcNumber == 1) {
        Donkos_RequestScheduling();
        //start a new process
    } else if (svcNumber == 2) {
        scheduler.RegisterProcess(process);
    }
}

