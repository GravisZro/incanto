TEMPLATE = app
CONFIG -= qt
CONFIG += c++14

QMAKE_CXXFLAGS += -std=c++14
QMAKE_CXXFLAGS += -pipe -Os
#QMAKE_CXXFLAGS += -fno-exceptions
QMAKE_CXXFLAGS += -fno-rtti
QMAKE_CXXFLAGS += -fno-threadsafe-statics

#LIBS += -lpthread

PUT = ../put
INCLUDEPATH += $$PUT

SOURCES = main.cpp

HEADERS += \
    basegen.h \
    putgen.h

# show these files but don't build them
showem {
SOURCES += \
    $$PUT/socket.cpp \
    $$PUT/process.cpp \
    $$PUT/application.cpp \
    $$PUT/cxxutils/vfifo.cpp \
    $$PUT/specialized/eventbackend.cpp \
    $$PUT/specialized/peercred.cpp \
    $$PUT/specialized/procstat.cpp

HEADERS += \
    $$PUT/socket.h \
    $$PUT/process.h \
    $$PUT/cxxutils/vfifo.h \
    $$PUT/cxxutils/posix_helpers.h \
    $$PUT/cxxutils/socket_helpers.h \
    $$PUT/cxxutils/error_helpers.h \
    $$PUT/cxxutils/hashing.h \
    $$PUT/object.h \
    $$PUT/application.h \
    $$PUT/specialized/eventbackend.h \
    $$PUT/specialized/peercred.h \
    $$PUT/specialized/procstat.h
}
