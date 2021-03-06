#-------------------------------------------------
#
# Project created by QtCreator 2016-06-18T23:57:03
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = simpleProcess
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    subwindowwidget.cpp \
    ../../qtsmartgraphics/qtsmartgraphics/qsmartgraphicsview.cpp \
    dialog/dialogblur.cpp \
    dialog/dialogfuzzylogic.cpp \
    dialog/dialogpreference.cpp \
    dialog/dialogresize.cpp \
    dialog/dialogsize.cpp \
    ../lib/imageprocessing.cpp \
    ../lib/imgcore.cpp \
    ../lib/imageprocessing/fft.cpp \
    ../lib/imageprocessing/fuzzylogic.cpp \
    ../lib/imageprocessing/spectralfiltering.cpp \
    ../libsources/qcustomplot.cpp \
    ../../qtsmartgraphics/qtsmartgraphics/qsmartlabel.cpp \
    plugindialog.cpp \
    tool/dialogcustomfilter.cpp \
    tool/dialogimagesubtracttool.cpp \
    dialog/dialogsinglebareffect.cpp

HEADERS  += mainwindow.h \
    subwindowwidget.h \
    ../../qtsmartgraphics/qtsmartgraphics/qsmartgraphicsview.h \
    plugininterfaces.h \
    dialog/dialogblur.h \
    dialog/dialogfuzzylogic.h \
    dialog/dialogpreference.h \
    dialog/dialogresize.h \
    dialog/dialogsize.h \
    ../lib/imageprocessing.h \
    ../lib/imgcore.h \
    ../lib/imageprocessing/fft.h \
    ../lib/imageprocessing/fuzzylogic.h \
    ../lib/imageprocessing/spectralfiltering.h \
    ../libsources/qcustomplot.h \
    ../../qtsmartgraphics/qtsmartgraphics/qsmartlabel.h \
    plugindialog.h \
    tool/dialogcustomfilter.h \
    tool/dialogimagesubtracttool.h \
    dialog/dialogsinglebareffect.h

FORMS    += mainwindow.ui \
    subwindowwidget.ui \
    dialog/dialogblur.ui \
    dialog/dialogfuzzylogic.ui \
    dialog/dialogpreference.ui \
    dialog/dialogresize.ui \
    dialog/dialogsize.ui \
    tool/dialogcustomfilter.ui \
    tool/dialogimagesubtracttool.ui \
    dialog/dialogsinglebareffect.ui

RESOURCES += \
    ../libsources/octicons/octicons.qrc \
    ../libsources/qdarkstyle/style.qrc \
    ../libsources/icon.qrc \
    ../../qtsmartgraphics/qtsmartgraphics/octicons/octicons.qrc

win32::LIBS += -lpsapi

msvc {
  QMAKE_CXXFLAGS += -openmp -arch:AVX -D "_CRT_SECURE_NO_WARNINGS"
  QMAKE_CXXFLAGS_RELEASE *= -O2
}

INCLUDEPATH += D:/Database/QtProject/qtsmartgraphics/qtsmartgraphics\
               $$quote(D:/libraries/opencv249o/include)\
               $$PWD\
               $$quote(../lib)\
               $$quote(../libsources)

#OPENCVLIB += $$quote(D:/libraries/opencv249o/x64/vc12/lib)

win32{
    !contains(QMAKE_TARGET.arch, x86_64){
         message("x86_64")
         OPENCVLIB += $$quote(D:/libraries/opencv249o/x86/vc12/lib)
    } else {
         message("x64 build")
         OPENCVLIB += $$quote(D:/libraries/opencv249o/x64/vc12/lib)
    }
}

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
