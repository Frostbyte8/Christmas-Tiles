#include "window_meterics.h"

WindowMetrics::WindowMetrics() : fontHDC(0), curFont(0), oldFont(0), avgCharWidth(0), avgCharHeight(0) {
}

void WindowMetrics::initWindowMetrics() {
    
    if(curFont) {
        return; // TODO: Updating these values.
    }

    NONCLIENTMETRICS ncm;
    ZeroMemory(&ncm, sizeof(ncm));
	ncm.cbSize = sizeof(ncm);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);

	OSVERSIONINFOEX osVer;
	ZeroMemory(&osVer, sizeof(osVer));
	osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    // The iPaddedBorderWidth method does not exist prior to Windows Vista
    #pragma warning( push )
	#pragma warning( disable: 4996)
	GetVersionEx(reinterpret_cast<OSVERSIONINFO *>(&osVer));
	#pragma warning( pop )

    if(osVer.dwMajorVersion < 6) {
		ncm.cbSize -= sizeof(ncm.iPaddedBorderWidth);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm) - sizeof(int),
							 &ncm, 0);
	}
	else {
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm),
							 &ncm, 0);
    }

    TEXTMETRIC tm;
    ZeroMemory(&tm, sizeof(TEXTMETRIC));

    fontHDC     = CreateCompatibleDC(NULL);
    curFont     = CreateFontIndirect(&ncm.lfMessageFont);
    oldFont     = reinterpret_cast<HFONT>(SelectObject(fontHDC, curFont));
    GetTextMetrics(fontHDC, &tm);
    
    // We only care about these two settings.
    avgCharWidth    = tm.tmAveCharWidth;
    avgCharHeight   = tm.tmHeight;


    // Margins / Spacing
    
    cs.XWINDOW_MARGIN               = XDLU2PIX(7);
    cs.YWINDOW_MARGIN               = YDLU2PIX(7);

    cs.XRELATED_MARGIN              = XDLU2PIX(4);
    cs.YRELATED_MARGIN              = YDLU2PIX(4);

    cs.XGROUPBOX_MARGIN             = XDLU2PIX(6);
    cs.YFIRST_GROUPBOX_MARGIN       = YDLU2PIX(11);
    cs.YLAST_GROUPBOX_MARGIN        = YDLU2PIX(7);

    // Dimensions

    cd.YLABEL                       = YDLU2PIX(8);

    cd.XBUTTON                     = XDLU2PIX(50);
    cd.YBUTTON                     = YDLU2PIX(14);

    cd.YTEXTBOX_ONE_LINE_ALONE     = YDLU2PIX(12);

    // A Few extras that are obtained via GetSystemMetrics
    cd.XSCROLLBAR                   = GetSystemMetrics(SM_CXVSCROLL);
    cd.YSCROLLBAR                   = GetSystemMetrics(SM_CYHSCROLL);

}

inline int WindowMetrics::XDLU2PIX(const int& x) const {
    return MulDiv(x, avgCharWidth, 4);
}

inline int WindowMetrics::YDLU2PIX(const int& y) const {
    return MulDiv(y, avgCharHeight, 8);
}

LONG WindowMetrics::calculateStringWidth(const wchar_t* str) const {
    const int numChars = wcslen(str);
    SIZE size;
    GetTextExtentPoint32(fontHDC, str, numChars, &size);
    return size.cx;
}