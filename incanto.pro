TEMPLATE = app
CONFIG -= qt
CONFIG += c++14
CONFIG += strict_c++
#CONFIG += exceptions_off
CONFIG += rtti_off

# universal arguments
QMAKE_CXXFLAGS += -fno-rtti

QMAKE_CXXFLAGS_DEBUG += -O0 -g3
QMAKE_CXXFLAGS_RELEASE += -Os


#QMAKE_CXXFLAGS_RELEASE += -fno-threadsafe-statics
QMAKE_CXXFLAGS_RELEASE += -fno-asynchronous-unwind-tables
#QMAKE_CXXFLAGS_RELEASE += -fstack-protector-all
QMAKE_CXXFLAGS_RELEASE += -fstack-protector-strong

# optimizations
QMAKE_CXXFLAGS_RELEASE += -fdata-sections
QMAKE_CXXFLAGS_RELEASE += -ffunction-sections
QMAKE_LFLAGS_RELEASE += -Wl,--gc-sections


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
    $$PUT/specialized/mutex.cpp \
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
    $$PUT/specialized/osdetect.h \
    $$PUT/specialized/eventbackend.h \
    $$PUT/specialized/mutex.h \
    $$PUT/specialized/peercred.h \
    $$PUT/specialized/procstat.h
}
