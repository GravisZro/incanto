TEMPLATE = app
CONFIG -= qt
CONFIG += c++14

QMAKE_CXXFLAGS += -std=c++14
QMAKE_CXXFLAGS += -pipe -Os
#QMAKE_CXXFLAGS += -fno-exceptions
QMAKE_CXXFLAGS += -fno-rtti
QMAKE_CXXFLAGS += -fno-threadsafe-statics

#LIBS += -lpthread

PDTK = ../pdtk
INCLUDEPATH += $$PDTK

SOURCES = main.cpp

HEADERS += \
    basegen.h \
    pdtkgen.h

# show these files but don't build them
showem {
SOURCES += \
    $$PDTK/socket.cpp \
    $$PDTK/process.cpp \
    $$PDTK/application.cpp \
    $$PDTK/cxxutils/vfifo.cpp \
    $$PDTK/specialized/eventbackend.cpp \
    $$PDTK/specialized/peercred.cpp \
    $$PDTK/specialized/procstat.cpp

HEADERS += \
    $$PDTK/socket.h \
    $$PDTK/process.h \
    $$PDTK/cxxutils/vfifo.h \
    $$PDTK/cxxutils/posix_helpers.h \
    $$PDTK/cxxutils/socket_helpers.h \
    $$PDTK/cxxutils/error_helpers.h \
    $$PDTK/cxxutils/hashing.h \
    $$PDTK/object.h \
    $$PDTK/application.h \
    $$PDTK/specialized/eventbackend.h \
    $$PDTK/specialized/peercred.h \
    $$PDTK/specialized/procstat.h
}
