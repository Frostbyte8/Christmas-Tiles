#ifndef __WINDOW_METRICS_H__
#define __WINDOW_METRICS_H__

#include <windows.h>
#include <string>

class WindowMetrics {

    public:

        struct ControlSpacing {

            friend class WindowMetrics;

            int XWINDOW_MARGIN;             // How much of a margin there is around a dialog window.
            int YWINDOW_MARGIN;

            int XLABELASSOC_MARGIN;         // Margin between a label and it's control.
            int YLABELASSOC_MARGIN;

            int XRELATED_MARGIN;            // Margin between related controls
            int YRELATED_MARGIN;
            
            int XUNRELATED_MARGIN;          // Same, but for unrelated controls.
            int YUNRELATED_MARGIN;

            int XGROUPBOX_MARGIN;           // Margin from the left/right side of a groupbox.
            int YFIRST_GROUPBOX_MARGIN;     // How far down from the top of the Groupbox the first control is.
            int YLAST_GROUPBOX_MARGIN;      // Space between bottom of the groupbox and the box's last control.

            int XGROUPBOX_SPACING;          // How much space between every control in a group box.
            int YGROUPBOX_SPACING;      

            int XBUTTON_MARGIN;             // Margins between buttons
            int YBUTTON_MARGIN;
               
            int YLABELSAMELINE_MARGIN;      // Text label beside a control

            int YLABEL_PARA_SPACE;          // Space between paragraph's of text.

            int XINTERACTIVE_MIN_SPACE;     // Smallest space between interactive controls.
            int YINTERACTIVE_MIN_SPACE;

            int XNONINTERACTIVE_MIN_SPACE;  // Smallest space between non-interactive controls.
            int YNONINTERACTIVE_MIN_SPACE;

            private:
                ControlSpacing() {};
        };

        struct ControlDimensions {
            
            friend class WindowMetrics;

            int XCHECKBOX;                  // Checkbox (and just only)
            int YCHECKBOX;
            int XCHECKBOX_GAP;              // Gap of Checkbox
            
            int YCOMBOBOX;

            int XBUTTON;                    // Command Buttons
            int YBUTTON;

            int YDROPDOWN;                  // Drop-Down List
            
            int XPROGRESSBAR_SHORT;         // Progress Bar
            int XPROGRESSBAR_LONG;
            int YPRORESSBAR;

            int XRADIOBUTTON;               // Radio Button (and just only)
            int YRADIOBUTTON;
            int XRADIOBUTTON_GAP;           // Gap of Checkbox

            int YLABEL;                     // Static Text Labels

            int YTEXTBOX_ONE_LINE_ALONE;    // Text Boxes
            int YTEXTBOX_ON_BUTTON_ROW;
			int YTEXTBOX_ADDITIONAL_LINES;
            
            int X_SCROLLBAR;                // Scroll Bars
            int Y_SCROLLBAR;
                         
            private:
                ControlDimensions() {};

        };

        WindowMetrics();
        ~WindowMetrics();
		
#ifdef __WIN9X_COMPAT__
        LONG CalculateStringWidth(const std::string& str) const;
#else 
        LONG CalculateStringWidth(const std::wstring& str) const;
#endif
        const ControlSpacing GetControlSpacing() const;
        const ControlDimensions GetControlDimensions() const;
        HFONT GetCurrentFont() const;
        int XDLU2PIX(const int& x) const;
        int YDLU2PIX(const int& y) const;

    private:

        HDC         fontHDC;
        HFONT       currentFont;
        HFONT       oldFont;
        LONG        avgCharWidth;
        LONG        avgCharHeight;

        ControlSpacing    cs;
        ControlDimensions cd;

        inline void InitWindowMetrics();

};

#endif // __WINDOWMETRICS_H__
