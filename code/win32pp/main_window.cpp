#include "main_window.h"

// GamePresenter only sets the member variable and does not use it, so we can
// safely ignore this warning.
#pragma warning( push )
#pragma warning( disable: 4355 )
MainWindow::MainWindow() : prevMonitor(0), gamePresenter(this) {
}
#pragma warning ( pop )

int MainWindow::OnCreate(CREATESTRUCT& cs) {
    UseThemes(FALSE);
    UseIndicatorStatus(FALSE);
    UseMenuStatus(FALSE);
    UseReBar(FALSE);
    UseToolBar(FALSE);
    UseOwnerDrawnMenu(FALSE);
    UseStatusBar(FALSE);
    CenterWindow();
    loadResources();

    uint8_t width    = GameConstants::DEF_WIDTH;
    uint8_t height   = GameConstants::DEF_HEIGHT;
    uint8_t numTypes = static_cast<uint8_t>(tileset->GetWidth() / tileset->GetHeight());
    gamePresenter.changeBoardSize(width, height, numTypes);

    return CFrame::OnCreate(cs);
}

HWND MainWindow::Create(HWND parent) {
    mainView.setWindowMetrics(&wMetrics);
    SetView(mainView);

    // Load Settings Here
    return CFrame::Create(parent);
}

void MainWindow::PreCreate(CREATESTRUCT& cs) {

    prevMonitor = MonitorFromWindow(*this, MONITOR_DEFAULTTONEAREST);

    MONITORINFO monitorInfo = {0};
    monitorInfo.cbSize = sizeof(monitorInfo);

    GetMonitorInfoW(prevMonitor, &monitorInfo); 

    cs.cx = cs.cy = 200;
    cs.x = monitorInfo.rcMonitor.left;
    cs.y = monitorInfo.rcMonitor.top;
    CFrame::PreCreate(cs);
}

//-----------------------------------------------------------------------------
// WndProc - Window Procedure for the Frame.
// Refer to the Win32++ documentation for more information.
//-----------------------------------------------------------------------------

LRESULT MainWindow::WndProc(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {

        case WM_EXITSIZEMOVE:
            // TODO: Only do this is a DPI change was registered.
            onWindowMoved();
            break;

        default:
            return WndProcDefault(msg, wParam, lParam);
    }

    return 0;
}


void MainWindow::onWindowMoved() {

    HMONITOR currentMonitor = MonitorFromWindow(*this, MONITOR_DEFAULTTONEAREST);

    if(currentMonitor != prevMonitor) {
        prevMonitor = currentMonitor;
        CenterWindow();
    }

}

//=============================================================================
// Public Interface Functions
//=============================================================================

//-----------------------------------------------------------------------------
// displayMessageBox - Displays a message box. See the implementation file
// for more information
//-----------------------------------------------------------------------------

void MainWindow::displayMessageBox(const uint32_t& langID) {
    //LOWORD(TITLE)
    //HIWORD(MSG)
    MessageBoxW(L"This will eventually contain a message", L"Ok", MB_OK);
}

//=============================================================================
// Private Functions
//=============================================================================

void MainWindow::loadResources() {
    // TODO: Error checking, custom sets, and so on.
    tileset = new Gdiplus::Bitmap(L"gfx.png");
    tileset->SetResolution(96, 96);
}
