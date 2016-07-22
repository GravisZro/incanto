TEMPLATE = app
CONFIG -= qt
#CONFIG += c++11

QMAKE_CXXFLAGS += -std=c++14
QMAKE_CXXFLAGS += -pipe -Os
#QMAKE_CXXFLAGS += -fno-exceptions
QMAKE_CXXFLAGS += -fno-rtti
QMAKE_CXXFLAGS += -fno-threadsafe-statics

LIBS += -lpthread

INCLUDEPATH += ../pdtk

SOURCES = main.cpp

HEADERS += \
    code_printer.h \
    code_printer/base.h \
    code_printer/cpp.h

# show these files but don't build them
showem {
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
    ../pdtk/specialized/getpeercred.h
}
