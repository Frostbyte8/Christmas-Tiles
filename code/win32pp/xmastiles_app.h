#pragma once
#ifndef __XMASTILES_APP_H__
#define __XMASTILES_APP_H__

#include "..\thirdparty\win32pp\wxx_appcore.h"
#include "main_window.h"

#include <gdiplus.h>

class XmasTilesApp : public CWinApp {

    public:

		XmasTilesApp() {}
		virtual ~XmasTilesApp() {}
		virtual BOOL InitInstance() {

            // We only need to start up GDI+. Shutting down is only required
            // we had an app that showed the UI from time to time, then we'd
            // shutdown and startup each time.
            Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

			mainWindowFrame.Create();
			return TRUE;
		}

	private:

		MainWindow mainWindowFrame;
        Gdiplus::GdiplusStartupInput    gdiplusStartupInput;
        ULONG_PTR                       gdiplusToken;

};

#endif // __XMASTILES_APP_H__
