#-------------------------------------------------
#
# Project created by QtCreator 2014-12-06T09:10:18
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = colorsplittool
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp\
        ../libsources/smartgraphicsview.cpp \
        ../libsources/smartlabel.cpp \
        ../lib/imageprocessing.cpp \
        ../lib/imgcore.cpp \
        ../lib/imageprocessing/fft.cpp \
        ../lib/imageprocessing/spectralfiltering.cpp \
        ../libsources/localsocketipc.cpp \
        ../libsources/sharememory.cpp \
        ../libsources/qlabelbar.cpp \
    split.cpp

HEADERS  += mainwindow.h\
        ../libsources/smartgraphicsview.h \
        ../libsources/smartlabel.h \
        ../lib/imageprocessing.h \
        ../lib/imgcore.h \
        ../lib/imageprocessing/fft.h \
        ../lib/imageprocessing/spectralfiltering.h \
        ../libsources/localsocketipc.h \
        ../libsources/sharememory.h \
        ../libsources/qlabelbar.h \
    split.h

FORMS    += mainwindow.ui

RESOURCES += \
    ../libsources/qdarkstyle/style.qrc \
    ../libsources/octicons/octicons.qrc \
    ../libsources/icon.qrc

win32::LIBS += -lpsapi

msvc {
  QMAKE_CXXFLAGS += -openmp -arch:AVX -D "_CRT_SECURE_NO_WARNINGS"
  QMAKE_CXXFLAGS_RELEASE *= -O2
}

INCLUDEPATH += $$quote(D:/libraries/opencv249o/include)\
               $$PWD\
               $$quote(../lib)\
               $$quote(../libsources)

OPENCVLIB += $$quote(D:/libraries/opencv249o/x64/vc12/lib)


CONFIG(debug, debug|release){
LIBS+= $$OPENCVLIB/opencv_core249d.lib\
       $$OPENCVLIB/opencv_highgui249d.lib\
       $$OPENCVLIB/opencv_ts249d.lib

}

CONFIG(release, debug|release){
LIBS+= $$OPENCVLIB/opencv_core249.lib\
       $$OPENCVLIB/opencv_highgui249.lib\
       $$OPENCVLIB/opencv_ts249.lib
}

OTHER_FILES += \
    ../libsources/simpleProcessing.rc
RC_FILE +=\
    ../libsources/simpleProcessing.rc

DEFINES += HAVE_OPENCV
