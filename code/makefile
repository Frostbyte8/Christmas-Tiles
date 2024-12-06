# NOTE
# main.exe: 1.cpp 2.cpp
# $@ evaluates to main.exe
# $< evaluates to 1.cpp
# $^ evaluates to 1.cpp 2.cpp

# Options
CPP     = cl
LNK     = link
LFLAGS  = /OUT:$@.exe /nologo /SUBSYSTEM:WINDOWS /DEBUG /DEFAULTLIB:user32.lib comctl32.lib Ole32.lib Gdi32.lib Advapi32.lib

# NOTE, THERE IS A SPACE AT THE END OF THIS LINE, DO NO REMOVE IT OR THE BUILD BREAKS
CFLAGS  = /nologo /c /Zi /FC /W4 /EHsc /DUNICODE /D_UNICODE /Fo..\build\ 

# Headers
WIN32_DIR       = ..\code\win32\window_metrics.h
WIN32PP_DIR     = ..\code\win32pp\main_window.h ..\code\win32pp\main_window_views.h ..\code\win32pp\xmastiles_app.h
PRESENTER_DIR	= ..\code\presenter\game_presenter.h
HEADERS         = $(WIN32_DIR) $(WIN32PP_DIR) $(PRESNTER_DIR)

OBJECTS = main.obj window_metrics.obj main_window.obj main_window_views.obj game_presenter.obj

default:
    @echo To build, run build.bat instead.

# OBJ to EXE
main: $(OBJECTS) $(HEADERS)
    $(LNK) $(LFLAGS) *.obj  

# CPP to OBJ

main.obj: ..\code\main.cpp
    $(CPP) $(CFLAGS) ..\code\main.cpp 

window_metrics.obj: ..\code\win32\window_metrics.cpp
    $(CPP) $(CFLAGS) ..\code\win32\window_metrics.cpp
    
main_window.obj: ..\code\win32pp\main_window.cpp
    $(CPP) $(CFLAGS) ..\code\win32pp\main_window.cpp
    
main_window_views.obj: ..\code\win32pp\main_window_views.cpp
    $(CPP) $(CFLAGS) ..\code\win32pp\main_window_views.cpp
       
game_presenter.obj: ..\code\presenter\game_presenter.cpp
    $(CPP) $(CFLAGS) ..\code\presenter\game_presenter.cpp
