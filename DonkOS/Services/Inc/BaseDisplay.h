#ifndef DONKOS_BASEDISPLAY_H
#define DONKOS_BASEDISPLAY_H

#include <cstdint>

class BaseDisplay {
public:
    virtual void Init() = 0;

    virtual void Display(int32_t page, int32_t line, const char *text) = 0;

    virtual void NextPage() = 0;

    virtual void Refresh() = 0;

    virtual void Clear() = 0;
};


#endif //DONKOS_BASEDISPLAY_H
