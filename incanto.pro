TEMPLATE = app
CONFIG -= qt
CONFIG += c++14

QMAKE_CXXFLAGS += -std=c++14
QMAKE_CXXFLAGS += -pipe -Os
#QMAKE_CXXFLAGS += -fno-exceptions
QMAKE_CXXFLAGS += -fno-rtti
QMAKE_CXXFLAGS += -fno-threadsafe-statics

#LIBS += -lpthread

INCLUDEPATH += ../pdtk

SOURCES = main.cpp

HEADERS += \
    basegen.h \
    pdtkgen.h

# show these files but don't build them
showem {
SOURCES += \
    ../pdtk/socket.cpp \
    ../pdtk/process.cpp \
    ../pdtk/application.cpp \
    ../pdtk/cxxutils/vfifo.cpp \
    ../pdtk/specialized/eventbackend.cpp \
    ../pdtk/specialized/peercred.cpp \
    ../pdtk/specialized/procstat.cpp

HEADERS += \
    ../pdtk/socket.h \
    ../pdtk/process.h \
    ../pdtk/cxxutils/vfifo.h \
    ../pdtk/cxxutils/posix_helpers.h \
    ../pdtk/cxxutils/socket_helpers.h \
    ../pdtk/cxxutils/error_helpers.h \
    ../pdtk/cxxutils/hashing.h \
    ../pdtk/object.h \
    ../pdtk/application.h \
    ../pdtk/specialized/eventbackend.h \
    ../pdtk/specialized/peercred.h \
    ../pdtk/specialized/procstat.h
}
