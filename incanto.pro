TEMPLATE = app
CONFIG = c++11

QMAKE_CXXFLAGS += -std=c++14
QMAKE_CXXFLAGS += -pipe -Os -fno-exceptions -fno-rtti -fno-threadsafe-statics
#QMAKE_CXXFLAGS += -pipe -Os
#QMAKE_CXXFLAGS += -fno-exceptions
#QMAKE_CXXFLAGS += -fno-rtti
#QMAKE_CXXFLAGS += -fno-threadsafe-statics

LIBS += -lpthread

INCLUDEPATH += ../pdtk

SOURCES += main.cpp

HEADERS += \
    code_printer.h \
    pdtk_printer.h

mac {
SOURCES += \
    ../pdtk/asocket.cpp \
    ../pdtk/application.cpp \
    ../pdtk/specialized/eventbackend.cpp \
    ../pdtk/specialized/getpeercred.cpp

HEADERS += \
    ../pdtk/cxxutils/vqueue.h \
    ../pdtk/asocket.h \
    ../pdtk/cxxutils/posix_helpers.h \
    ../pdtk/cxxutils/socket_helpers.h \
    ../pdtk/cxxutils/error_helpers.h \
    ../pdtk/cxxutils/hashing.h \
    ../pdtk/object.h \
    ../pdtk/application.h \
    ../pdtk/rpc.h \
    ../pdtk/specialized/eventbackend.h \
    ../pdtk/specialized/getpeercred.h \
}
