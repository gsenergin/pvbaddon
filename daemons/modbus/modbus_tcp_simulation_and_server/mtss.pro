######################################################################
# generated by pvdevelop at: Mo. Aug 8 12:23:57 2011
######################################################################

TEMPLATE = app
CONFIG  += warn_on release console
CONFIG  -= qt

# Input
# HEADERS +=
SOURCES += main.cpp

!macx {
unix:LIBS          += -lpthread
unix:LIBS         += /usr/lib/librllib.so
unix:INCLUDEPATH  += /opt/pvb/rllib/lib
}

macx:LIBS          += /usr/lib/libpthread.dylib
macx:LIBS         += /usr/lib/librllib.dylib
macx:INCLUDEPATH  += /opt/pvb/rllib/lib

win32-g++ {
QMAKE_LFLAGS      += -static-libgcc
win32:LIBS        += "$(PVBDIR)/win-mingw/bin/librllib.a"
win32:LIBS        += -lws2_32 -ladvapi32
win32:INCLUDEPATH += "$(PVBDIR)/rllib/lib"
}
else {
win32:LIBS        += wsock32.lib advapi32.lib
win32:LIBS        += "$(PVBDIR)/win/bin/rllib.lib"
win32:INCLUDEPATH += "$(PVBDIR)/rllib/lib"
}

TARGET = mtss
