TEMPLATE = lib

INCLUDEPATH += ../../sv-dependency-builds/win32-mingw/include ../../../boost_1_57_0
LIBS += ../../sv-dependency-builds/win32-mingw/lib/libvamp-sdk.a -Wl,--version-script=../win32-build/vamp-plugin.map

CONFIG -= qt
CONFIG += plugin release warn_on

TARGET = pyin

SOURCES += \
    ../YinUtil.cpp \
    ../Yin.cpp \
    ../SparseHMM.cpp \
    ../MonoPitchHMM.cpp \
    ../MonoPitch.cpp \
    ../MonoNoteParameters.cpp \
    ../MonoNoteHMM.cpp \
    ../MonoNote.cpp \
    ../libmain.cpp \
    ../YinVamp.cpp \
    ../PYinVamp.cpp \
    ../LocalCandidatePYIN.cpp

HEADERS += \
    ../YinUtil.h \
    ../Yin.h \
    ../SparseHMM.h \
    ../MonoPitchHMM.h \
    ../MonoPitch.h \
    ../MonoNoteParameters.h \
    ../MonoNoteHMM.h \
    ../MonoNote.h \
    ../MeanFilter.h \
    ../YinVamp.h \
    ../PYinVamp.h \
    ../LocalCandidatePYIN.h

