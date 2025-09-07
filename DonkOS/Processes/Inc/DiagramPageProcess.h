#ifndef DONKOS_DIAGRAMPAGEPROCESS_H
#define DONKOS_DIAGRAMPAGEPROCESS_H

#include "Process.h"
#include "LedDisplay.h"

class DiagramPageProcess : public Process {
public:
    explicit DiagramPageProcess(LedDisplay *display);

    void Main() override;

    void PutData(float temperature);

private:
    static constexpr int32_t offset = 20;
    static constexpr int32_t data_capacity = 128 - offset;

    LedDisplay *display;
    float temp_data_over_time[data_capacity];
    int32_t index;
};


#endif //DONKOS_DIAGRAMPAGEPROCESS_H
