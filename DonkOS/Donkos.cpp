#include "Donkos.h"

#include <cstdio>

#include "Process7SegmentDisplay.h"
#include "ProcessLed.h"
#include "RoundRobinScheduler.h"
#include "ADC3Process.h"
#include "DisplayRefreshProcess.h"
#include "LedDisplay.h"
#include "DonkosInternal.h"
#include "DHT11NonblockingProcess2.h"
#include "dwt_delay.h"
#include "DiagramPageProcess.h"
#include "GPSProcess.h"
#include "DonkosLogger.h"
#include "WiFiProcess.h"

namespace {
    Process7SegmentDisplay mutexProcess{};
    ProcessLed ledProcess{};

    LedDisplay display{};
    DisplayRefreshProcess displayRefreshProcess{&display};
    DHT11NonblockingProcess2 dht11NonblockingProcess2{};
    DiagramPageProcess diagramProcess{&display};
    ADC3Process adcProcess{&diagramProcess};
    GPSProcess gps_process{&display};
    WiFiProcess wifi_process{};

    RoundRobinScheduler scheduler{};
}

extern ADC_HandleTypeDef hadc3;
extern I2C_HandleTypeDef hi2c1;
extern TIM_HandleTypeDef htim7;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart2;

volatile uint32_t CUBE_DBG_CURRENT_PROCESS = 0;
volatile int32_t CUBE_DBG_ACTIVE_SVC = -1;

void Donkos_Main() {
    Logger_Debug("[DBG] Donkos_Main...\n");

    SCB->CCR |= SCB_CCR_STKALIGN_Msk;
    DWT_Init();


    adcProcess.SetHandle(hadc3);
    display.SetHandle(hi2c1);

    Logger_Debug("[DBG] Starting processes...\n");

    Donkos_StartNewProcess(&mutexProcess);
    Donkos_StartNewProcess(&ledProcess);
    Donkos_StartNewProcess(&adcProcess);
    Donkos_StartNewProcess(&displayRefreshProcess);
    Donkos_StartNewProcess(&gps_process);
    Donkos_StartNewProcess(&wifi_process);

    scheduler.SetInitialProcess(&mutexProcess);

    __set_CONTROL(0x3); //unprivileged mode starting here
    __ISB();
    __WFI();

    while (true) {
        __NOP();
    }
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
#ifdef Debug
    CUBE_DBG_CURRENT_PROCESS = scheduler.GetCurrentProcess()->GetPid();
#endif
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
#ifdef Debug
    CUBE_DBG_ACTIVE_SVC = static_cast<int32_t>(svcNumber);
#endif
    if (svcNumber == ServiceCall::UNREGISTER_RESCHEDULE) {
        scheduler.UnregisterProcess(process);
        Donkos_RequestScheduling();
    } else if (svcNumber == ServiceCall::RESCHEDULE) {
        Donkos_RequestScheduling();
    } else if (svcNumber == ServiceCall::START_PROCESS) {
        scheduler.RegisterProcess(process);
    }
#ifdef Debug
    CUBE_DBG_ACTIVE_SVC = -1;
#endif
}


void Donkos_KeyPressed(int32_t keyId) {
    Logger_Debug("[DBG] Donkos_KeyPressed: keyId = %ld\n", keyId);

    if (display.GetCurrentPageIndex() == 1) {
        scheduler.RegisterProcess(&dht11NonblockingProcess2);
    } else if (display.GetCurrentPageIndex() == 2) {
        scheduler.UnregisterProcess(&dht11NonblockingProcess2);
        scheduler.RegisterProcess(&diagramProcess);
    } else if (display.GetCurrentPageIndex() == 3) {
        scheduler.UnregisterProcess(&diagramProcess);
    }

    display.NextPage();
}

int32_t Donkos_GetSystemState() {
    return 0;
}

void Donkos_ClearDisplay() {
    display.Clear();
}

void Donkos_TimerElapsed(int32_t timerId) {
    //dht11NonblockingProcess.TimerInterruptReceived();
}

void Donkos_ExternalInterruptReceived(int32_t id) {
    dht11NonblockingProcess2.InterruptReceived();
}

extern "C" int __io_putchar(int ch) __attribute__((weak));

int __io_putchar(int ch) {
    HAL_UART_Transmit(&huart2, (uint8_t *) &ch, 1, 100);
    return ch;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    if (huart->Instance == huart4.Instance) {
        gps_process.Reset();
        gps_process.UartReceived(Size);
        scheduler.RegisterProcess(&gps_process);
    }
}
