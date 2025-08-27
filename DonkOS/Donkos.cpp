#include "Donkos.h"
#include "Process7SegmentDisplay.h"
#include "ProcessLed1.h"
#include "ProcessLed2.h"
#include "RoundRobinScheduler.h"
#include "ADC3Process.h"
#include "DisplayRefreshProcess.h"
#include "LedDisplay.h"
#include "DonkosInternal.h"
#include "DHT11Process.h"
#include "DHT11NonblockingProcess.h"
#include "DHT11NonblockingProcess2.h"
#include "dwt_delay.h"


namespace {

    Process7SegmentDisplay mutexProcess{};
    ProcessLed1 led1Process{};
    ProcessLed2 led2Process{};
    ADC3Process adcProcess{};
    DHT11Process dht11Process{};

    LedDisplay display{};
    DisplayRefreshProcess displayRefreshProcess{&display};
    DHT11NonblockingProcess dht11NonblockingProcess{};
    DHT11NonblockingProcess2 dht11NonblockingProcess2{};

    RoundRobinScheduler scheduler{};
}

extern ADC_HandleTypeDef hadc3;
extern I2C_HandleTypeDef hi2c1;
extern TIM_HandleTypeDef htim7;

void Donkos_Main() {
    SCB->CCR |= SCB_CCR_STKALIGN_Msk;
    DWT_Init();

    adcProcess.SetHandle(hadc3);
    display.SetHandle(hi2c1);
    dht11Process.SetHandle(htim7);
    dht11NonblockingProcess.SetHandle(htim7);

    scheduler.RegisterProcess(&mutexProcess);
    scheduler.RegisterProcess(&led1Process);
    scheduler.RegisterProcess(&led2Process);
    scheduler.RegisterProcess(&adcProcess);
    scheduler.RegisterProcess(&displayRefreshProcess);
    scheduler.RegisterProcess(&dht11NonblockingProcess2);
    //scheduler.RegisterProcess(&dht11Process);
    //scheduler.RegisterProcess(&dht11NonblockingProcess);

    scheduler.SetInitialProcess(&mutexProcess);

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


void Donkos_Display(int32_t page, int32_t line, const char *text) {
    display.Display(page, line, text);
}

void Donkos_GenericProcessMain() {
    Process *p = scheduler.GetCurrentProcess();
    p->Main();
    Donkos_EndProcess(p);
}


void Donkos_ServiceHandler(ServiceCall svcNumber, Process *process) {
    __disable_irq();

    if (svcNumber == ServiceCall::UNREGISTER_RESCHEDULE) {
        scheduler.UnregisterProcess(process);
        Donkos_RequestScheduling();
    } else if (svcNumber == ServiceCall::RESCHEDULE) {
        Donkos_RequestScheduling();
    } else if (svcNumber == ServiceCall::START_PROCESS) {
        scheduler.RegisterProcess(process);
    }

    __enable_irq();
}

void Donkos_KeyPressed(int32_t keyId) {
    display.NextPage();
}

int32_t Donkos_GetSystemState() {
    return 0;
}

void Donkos_ClearDisplay() {
    display.Clear();
}

void Donkos_TimerElapsed(int32_t timerId) {
    dht11NonblockingProcess.TimerInterruptReceived();
}

void Donkos_ExternalInterruptReceived(int32_t id) {
    dht11NonblockingProcess2.InterruptReceived();
}

