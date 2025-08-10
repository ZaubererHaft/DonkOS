#ifndef TEST_DACPROCESS_H
#define TEST_DACPROCESS_H

#include "Process.h"
#include "main.h"


class DACProcess : public Process  {
public:
    DACProcess();

    void Init();

    void Main() override;

private:
    DAC_HandleTypeDef hdac1;
};


#endif //TEST_DACPROCESS_H
