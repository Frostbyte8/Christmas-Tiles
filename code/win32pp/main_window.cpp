#include "main_window.h"

MainWindow::MainWindow() : prevMonitor(0), gamePresenter(this) {
}

int MainWindow::OnCreate(CREATESTRUCT& cs) {
    UseThemes(FALSE);
    UseIndicatorStatus(FALSE);
    UseMenuStatus(FALSE);
    UseReBar(FALSE);
    UseToolBar(FALSE);
    UseOwnerDrawnMenu(FALSE);
    UseStatusBar(FALSE);
    CenterWindow();
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
