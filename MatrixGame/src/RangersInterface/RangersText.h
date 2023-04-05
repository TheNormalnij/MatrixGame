
#pragma once

#include "../MatrixGameDll.hpp"

class RangersText {
public:
    RangersText() = default;
    ~RangersText() = default;

    void createText(wchar_t *text, wchar_t *font, uint32_t color, int sizex, int sizey, int alignx, int aligny,
                     int wordwrap, int smex, int smy, Base::CRect *clipr, SMGDRangersInterfaceText *it);

    void destroyText(SMGDRangersInterfaceText *it);
};