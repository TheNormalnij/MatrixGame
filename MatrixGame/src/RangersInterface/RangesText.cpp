
#include "RangersText.h"
#include <stdio.h>

void RangersText::createText(wchar_t *text, wchar_t *font, uint32_t color, int sizex, int sizey,
                                           int alignx, int aligny, int wordwrap, int smex, int smy, Base::CRect *clipr,
                                           SMGDRangersInterfaceText *it) {
    printf("[STUB] createText '%ls' %p", text, it);
}

void RangersText::destroyText(SMGDRangersInterfaceText *it) {
    printf("[STUB] destroyText %p", it);
}
