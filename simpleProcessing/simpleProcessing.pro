#-------------------------------------------------
#
# Project created by QtCreator 2014-10-16T20:09:22
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = simpleProcessing
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dialog/dialogresize.cpp \
    dialog/dialogblur.cpp \
    dialog/dialogpreference.cpp \
    dialog/dialogsize.cpp \
    dialog/dialogfuzzylogic.cpp \
    tool/dialogcustomfilter.cpp \
    tool/dialogimagesubtracttool.cpp \
    tool/spectralfiltertool.cpp \
    ../lib/imageprocessing.cpp \
    ../lib/imgcore.cpp \
    ../lib/imageprocessing/fft.cpp \
    ../lib/imageprocessing/fuzzylogic.cpp \
    ../lib/imageprocessing/spectralfiltering.cpp \
    ../resources/qcustomplot.cpp \
    ../resources/smartgraphicsview.cpp \
    ../resources/smartlabel.cpp \
    ../resources/localsocketipc.cpp\
    ../resources/sharememory.cpp

HEADERS  += mainwindow.h \
    dialog/dialogresize.h \
    dialog/dialogblur.h \
    dialog/dialogpreference.h \
    dialog/dialogsize.h \
    dialog/dialogfuzzylogic.h \
    tool/dialogcustomfilter.h \
    tool/dialogimagesubtracttool.h \
    tool/spectralfiltertool.h \
    ../lib/imageprocessing.h \
    ../lib/imgcore.h \
    ../lib/imageprocessing/fft.h \
    ../lib/imageprocessing/fuzzylogic.h \
    ../lib/imageprocessing/spectralfiltering.h \
    ../resources/qcustomplot.h \
    ../resources/smartgraphicsview.h \
    ../resources/smartlabel.h \
    ../resources/localsocketipc.h \
    ../resources/sharememory.h

FORMS    += mainwindow.ui \
    dialog/dialogblur.ui \
    dialog/dialogpreference.ui \
    dialog/dialogresize.ui \
    dialog/dialogsize.ui \
    dialog/dialogfuzzylogic.ui \
    tool/dialogcustomfilter.ui \
    tool/dialogimagesubtracttool.ui \
    tool/spectralfiltertool.ui

RESOURCES += \
    ../resources/octicons/octicons.qrc \
    ../resources/qdarkstyle/style.qrc \
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



