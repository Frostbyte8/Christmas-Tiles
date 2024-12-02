#ifndef __XMASTILES_APP_H__
#define __XMASTILES_APP_H__

#include "..\thirdparty\win32pp\wxx_appcore.h"
#include "main_window.h"

class XmasTilesApp : public CWinApp {

    public:

		XmasTilesApp() {}
		virtual ~XmasTilesApp() {}
		virtual BOOL InitInstance() {
			mainWindowFrame.Create();
			return TRUE;
		}

	private:
		MainWindow mainWindowFrame;

};

#endif // __XMASTILES_APP_H__
