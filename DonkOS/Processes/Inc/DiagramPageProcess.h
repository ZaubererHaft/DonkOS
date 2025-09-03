#ifndef DONKOS_DIAGRAMPAGEPROCESS_H
#define DONKOS_DIAGRAMPAGEPROCESS_H

#include "BaseDisplay.h"
#include "Process.h"

class DiagramPageProcess : public Process {
public:
    explicit DiagramPageProcess(BaseDisplay *display);

    void Main() override;

private:
    BaseDisplay *display;
};


#endif //DONKOS_DIAGRAMPAGEPROCESS_H
