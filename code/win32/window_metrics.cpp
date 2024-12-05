#include "window_metrics.h"

// TODO LIST:
// DPI Scaling, if necessary
// An option to change the font
// An option to specify a different font on creation
// InitWindowMetrics should be update window metrics

WindowMetrics::WindowMetrics() : fontHDC(NULL), currentFont(NULL), 
                                 oldFont(NULL), avgCharWidth(0),
                                 avgCharHeight(0) {

    // Obtain LOGFONT
    NONCLIENTMETRICS ncm;
    ZeroMemory(&ncm, sizeof(ncm));
	ncm.cbSize = sizeof(ncm);

	OSVERSIONINFOEX osVer;
	ZeroMemory(&osVer, sizeof(osVer));
	osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	// Starting with Visual Studio 2008, NONCLIENTMETRICS got an extra integer.
	// So we need to shave that off. However, this check isn't necessary
	// if we are compiling with an older MSVC like 2005, which contains
	// the old size.

	// The shaving of the extra integer is only necessary if the user is running
	// a version of Windows earlier than Vista.

    // TODO: Research this more, and verify it doesn't break.

#if _MSC_VER < 1500
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
#else

	// We're going to use this anyways because were not brain dead. :)
    // Also, I spent a good 30 minutes trying to get the other method to work,
    // and it did not, and there are more important things to waste time on.

	#pragma warning( push )
	#pragma warning( disable: 4996)
	GetVersionEx(reinterpret_cast<OSVERSIONINFO *>(&osVer));
	#pragma warning( pop )

	// TODO: There is a DPI compatible version of this in newer VCs. 

	if(osVer.dwMajorVersion < 6) {
		ncm.cbSize -= sizeof(int);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm) - sizeof(int),
							 &ncm, 0);
	}
	else {
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm),
							 &ncm, 0);
	}

#endif // _MSC_VER


    // Get Necessary Metrics to determine Dialog Metrics
    TEXTMETRIC tm;
    ZeroMemory(&tm, sizeof(TEXTMETRIC));

    fontHDC     = CreateCompatibleDC(NULL);
    currentFont = CreateFontIndirect(&ncm.lfMessageFont);
    oldFont     = reinterpret_cast<HFONT>(SelectObject(fontHDC, currentFont));
    GetTextMetrics(fontHDC, &tm);
    
    // We only care about these two settings.
    avgCharWidth    = tm.tmAveCharWidth;
    avgCharHeight   = tm.tmHeight;

    InitWindowMetrics();

}

WindowMetrics::~WindowMetrics() {

    // Release resources, if we accquired them
    if(currentFont && fontHDC) {
        
        // Reset our HDC back to the way we had it before we swapped out
        // the font.
        SelectObject(fontHDC, oldFont);
        DeleteObject(currentFont);
        currentFont = NULL;

        // And delete our DC.
        DeleteDC(fontHDC);
        fontHDC = NULL;

    }

}

///----------------------------------------------------------------------------
/// GetDimensions - Returns a copy of the DialogControlDimensions structure.
/// @return a copy of a DialogControlDimensions structure.
///----------------------------------------------------------------------------

const WindowMetrics::ControlDimensions WindowMetrics::GetControlDimensions() const {
    return cd;
}

///----------------------------------------------------------------------------
/// GetSpacing - Returns a copy of the DialogControlSpacing structure.
/// @return a copy of a DialogControlSpacing structure.
///----------------------------------------------------------------------------

const WindowMetrics::ControlSpacing WindowMetrics::GetControlSpacing() const {
    return cs;
}

///----------------------------------------------------------------------------
/// InitWindowMetrics - Set the values each measurement based on the current
/// font. This is here instead of the constructor for easier reading.
///----------------------------------------------------------------------------

inline void WindowMetrics::InitWindowMetrics() {

    // Margins and Spacing

    cs.XWINDOW_MARGIN              = XDLU2PIX(7);
    cs.YWINDOW_MARGIN              = YDLU2PIX(7);

    cs.XLABELASSOC_MARGIN          = XDLU2PIX(3);
    cs.YLABELASSOC_MARGIN          = YDLU2PIX(3);

    cs.XRELATED_MARGIN             = XDLU2PIX(4);
    cs.YRELATED_MARGIN             = YDLU2PIX(4);

    cs.XUNRELATED_MARGIN           = XDLU2PIX(7);
    cs.YUNRELATED_MARGIN           = YDLU2PIX(7);

    cs.XGROUPBOX_MARGIN            = XDLU2PIX(6);

    cs.YFIRST_GROUPBOX_MARGIN      = YDLU2PIX(11);
    cs.YLAST_GROUPBOX_MARGIN       = YDLU2PIX(7);

    cs.XGROUPBOX_SPACING           = XDLU2PIX(4);
    cs.YGROUPBOX_SPACING           = YDLU2PIX(4);

    cs.XBUTTON_MARGIN              = XDLU2PIX(4);
    cs.YBUTTON_MARGIN              = YDLU2PIX(4);

    cs.YLABELSAMELINE_MARGIN       = YDLU2PIX(3);
    
    cs.YLABEL_PARA_SPACE           = YDLU2PIX(3);

    cs.XINTERACTIVE_MIN_SPACE      = XDLU2PIX(3);
    cs.YINTERACTIVE_MIN_SPACE      = XDLU2PIX(3);

    cs.XNONINTERACTIVE_MIN_SPACE   = XDLU2PIX(2);
    cs.YNONINTERACTIVE_MIN_SPACE   = XDLU2PIX(2);

    // Control Sizes
    // After much testing, it seems that 1 average character
    // length tends to be how wide the gap is.
    cd.XCHECKBOX                   = XDLU2PIX(10);
    cd.YCHECKBOX                   = YDLU2PIX(10);
    cd.XCHECKBOX_GAP               = XDLU2PIX(4);

    cd.YCOMBOBOX                   = YDLU2PIX(14);

    cd.XBUTTON                     = XDLU2PIX(50);
    cd.YBUTTON                     = YDLU2PIX(14);
    
    cd.YDROPDOWN                   = YDLU2PIX(14);

    cd.XPROGRESSBAR_SHORT          = XDLU2PIX(107);
    cd.XPROGRESSBAR_LONG           = XDLU2PIX(237);
    cd.YPRORESSBAR                 = YDLU2PIX(8);

    cd.XRADIOBUTTON                = XDLU2PIX(10);
    cd.YRADIOBUTTON                = YDLU2PIX(10);
    cd.XRADIOBUTTON_GAP            = XDLU2PIX(4);

    cd.YLABEL                      = YDLU2PIX(8);
    
    // MSDN says to use 14, however it seems that many applications, MS 
    // applications included, use 12 if the button is alone, and only use the 14
    // that is documented when the textbox is on the same row as a button,
	// 14 just so happens to be the same height as a button.

    // You can see this for yourself in the Internet Options dialog box: The
    // the text boxes are 12 DLUs (20px~), while the one in the Accessibility
    // sub-dialog for the style sheet is 14 (23px~) since it's on the same row
    // as a button! Maybe MSDN is wrong? Maybe they forgot to mention this?

    cd.YTEXTBOX_ONE_LINE_ALONE     = YDLU2PIX(12);
    cd.YTEXTBOX_ON_BUTTON_ROW      = cd.YBUTTON; // Make it consistent.
	cd.YTEXTBOX_ADDITIONAL_LINES   = YDLU2PIX(10);   

    // A Few extras that are obtained via GetSystemMetrics
    cd.X_SCROLLBAR                 = GetSystemMetrics(SM_CXVSCROLL);
    cd.Y_SCROLLBAR                 = GetSystemMetrics(SM_CYHSCROLL);

}

///----------------------------------------------------------------------------
/// XDLU2PIX - Converts horizontal dialog units into pixels
/// @param int number of horizontal dialog units 
/// @return int number of pixels
///----------------------------------------------------------------------------

int WindowMetrics::XDLU2PIX(const int& x) const {
    return MulDiv(x, avgCharWidth, 4);
}

///----------------------------------------------------------------------------
/// YDLU2PIX - Converts vertical dialog units into pixels
/// @param int number of vertical dialog units 
/// @return int number of pixels
///----------------------------------------------------------------------------

int WindowMetrics::YDLU2PIX(const int& y) const {
    return MulDiv(y, avgCharHeight, 8);
}

HFONT WindowMetrics::GetCurrentFont() const {
    return currentFont;
}

///----------------------------------------------------------------------------
/// CalculateStringWidth - Calculate the width of a string based on the current
/// font in use.
/// @param std::wstring The string to calculate
/// @return int number of pixels
///----------------------------------------------------------------------------

#ifdef __WIN9X_COMPAT__
LONG WindowMetrics::CalculateStringWidth(const std::string& str) const {
#else
LONG WindowMetrics::CalculateStringWidth(const std::wstring& str) const {
#endif
    SIZE size;
    GetTextExtentPoint32(fontHDC, str.c_str(), str.length(), &size);
    return size.cx;
}
