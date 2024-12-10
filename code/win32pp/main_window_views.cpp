#include "main_window_views.h"

namespace GroupBox {
    enum {
        Score,
        Points,
        Time,
        NUM_BOXES
    };
}

static const int GRP_SCORE      = 0;
static const int GRP_POINTS     = 1;
static const int GRP_TIME       = 2;

// TODO: WindowMetrics needs to be constant.
void MainWindowView::setWindowMetrics(WindowMetrics* inMetrics) {
    wMetrics = inMetrics;
}

int MainWindowView::OnCreate(CREATESTRUCT&) {

    HFONT dialogFont = wMetrics->GetCurrentFont();
    // TODO: Update font when metrics change
    for(int i = 0; i < GroupBox::NUM_BOXES; ++i) {
        grpInfo[i].Create(*this, 0, BS_GROUPBOX);
        grpInfo[i].SendMessage(WM_SETFONT, reinterpret_cast<LPARAM>(dialogFont), TRUE);
    }

    btnPause.Create(*this, 0, BS_PUSHBUTTON);

    grpInfo[GroupBox::Score].SetWindowText(L"Score");
    grpInfo[GroupBox::Points].SetWindowText(L"Points");
    grpInfo[GroupBox::Time].SetWindowText(L"Time");
    btnPause.SetWindowText(L"Pause");
    btnPause.SendMessage(WM_SETFONT, reinterpret_cast<LPARAM>(dialogFont), TRUE);


    sizeControls();

    return 0;
}

void MainWindowView::sizeControls() {

    // TODO: Tempvars
    // TODO: DPI Scaling
    const WORD minWidth = 160;  // Min Width of board
    const WORD minHeight = 288; // Min Height of client area

    const WindowMetrics::ControlSpacing sp = wMetrics->GetControlSpacing();

    const WORD sideBarMinWidth = wMetrics->CalculateStringWidth(L"Points") + (sp.XGROUPBOX_MARGIN * 2);
    CRect sideBarDims(0, 0, sideBarMinWidth, 288 / 4);

    for(int i = 0; i < GroupBox::NUM_BOXES; ++i) {
        grpInfo[i].MoveWindow( sideBarDims.left, sideBarDims.top, sideBarDims.Width(), sideBarDims.Height());
        sideBarDims.OffsetRect(0, sideBarDims.Height());
    }

    // TODO: The Pause button uses up the remaining height
    btnPause.MoveWindow(sideBarDims.left, sideBarDims.top, sideBarDims.Width(), sideBarDims.Height());
}
