#ifndef TEST_SSD1306PROCESS_H
#define TEST_SSD1306PROCESS_H

#include "Process.h"
#include "main.h"
#include "BaseDisplay.h"

class DisplayRefreshProcess  : public Process {
public:
    explicit DisplayRefreshProcess(BaseDisplay *display);

    void Main() override;

private:
    BaseDisplay *display;
};



#endif //TEST_SSD1306PROCESS_H
