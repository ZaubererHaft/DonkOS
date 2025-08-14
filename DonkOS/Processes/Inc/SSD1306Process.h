//
// Created by ludwig on 8/26/24.
//

#ifndef TEST_SSD1306PROCESS_H
#define TEST_SSD1306PROCESS_H

#include "Process.h"

// I2C
class SSD1306Process  : public Process {
public:
    void Init();

    void Main() override;
};



#endif //TEST_SSD1306PROCESS_H
