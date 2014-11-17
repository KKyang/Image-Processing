#-------------------------------------------------
#
# Project created by QtCreator 2014-11-17T23:30:59
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = spectralfilteringtool
TEMPLATE = app


SOURCES += main.cpp\
        spectralfiltertool.cpp \
    ../resources/qcustomplot.cpp \
    ../resources/smartgraphicsview.cpp \
    ../resources/smartlabel.cpp \
    ../lib/imageprocessing.cpp \
    ../lib/imgcore.cpp \
    ../lib/imageprocessing/fft.cpp \
    ../lib/imageprocessing/spectralfiltering.cpp

HEADERS  += spectralfiltertool.h \
    ../resources/qcustomplot.h \
    ../resources/smartgraphicsview.h \
    ../resources/smartlabel.h \
    ../lib/imageprocessing.h \
    ../lib/imgcore.h \
    ../lib/imageprocessing/fft.h \
    ../lib/imageprocessing/spectralfiltering.h


FORMS    += spectralfiltertool.ui

RESOURCES += \
    ../resources/qdarkstyle/style.qrc \
    ../resources/octicons/octicons.qrc \
    ../resources/icon.qrc

msvc {
  QMAKE_CXXFLAGS += -openmp -arch:AVX -D "_CRT_SECURE_NO_WARNINGS"
  QMAKE_CXXFLAGS_RELEASE *= -O2
}

INCLUDEPATH += $$quote(D:/libraries/opencv249o/include)\
               $$PWD\
               $$quote(../lib)\
               $$quote(../resources)

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
    ../resources/simpleProcessing.rc
RC_FILE +=\
    ../resources/simpleProcessing.rc
