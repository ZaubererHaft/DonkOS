#ifndef DONKOS_BASEDISPLAY_H
#define DONKOS_BASEDISPLAY_H

#include <cstdint>
#include <utility>

class BaseDisplay {
public:
    virtual bool Init() = 0;

    virtual void Display(int32_t page, int32_t line, const char *text) = 0;

    virtual void NextPage() = 0;

    virtual void Refresh() = 0;

    virtual void Clear() = 0;

    virtual void DrawPixel(int32_t x, int32_t y) = 0;

    virtual std::pair<int32_t, int32_t> GetDimensions() = 0;

    virtual int32_t GetCurrentPageIndex() const = 0;

    virtual void WriteAt(int32_t x, int32_t y, const char *text) = 0;

    virtual void SetCurrentPageDirty() = 0;
};


#endif //DONKOS_BASEDISPLAY_H
