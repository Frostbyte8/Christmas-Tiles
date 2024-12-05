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

void MainWindowView::setWindowMetrics(WindowMetrics* inMetrics) {
    wMetrics = inMetrics;
}

int MainWindowView::OnCreate(CREATESTRUCT& cs) {

    for(int i = 0; i < GroupBox::NUM_BOXES; ++i) {

        grpInfo[i].Create(*this, 0, BS_GROUPBOX);
    }

    grpInfo[GroupBox::Score].MoveWindow(0, 0, 128, 128, 0);
    grpInfo[GroupBox::Score].SetWindowText(L"Score");

    HFONT dialogFont = wMetrics->GetCurrentFont();
    grpInfo[GroupBox::Score].SendMessage(WM_SETFONT, reinterpret_cast<LPARAM>(dialogFont), TRUE);
    
    return 0;
}
