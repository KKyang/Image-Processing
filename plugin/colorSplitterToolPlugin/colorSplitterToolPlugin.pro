#-------------------------------------------------
#
# Project created by QtCreator 2016-06-18T10:34:35
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TEMPLATE = lib
CONFIG += plugin



SOURCES += \
    mainwindow.cpp \
    split.cpp \
    ../../../qtsmartgraphics/qtsmartgraphics/qsmartgraphicsview.cpp \
    ../../../qtsmartgraphics/qtsmartgraphics/qsmartlabel.cpp \
    ../../libsources/qlabelbar.cpp \
    ../../lib/imageprocessing.cpp \
    ../../lib/imgcore.cpp \
    colorsplittertoolplugin.cpp

HEADERS += \
    mainwindow.h \
    split.h \
    ../../../qtsmartgraphics/qtsmartgraphics/qsmartgraphicsview.h \
    ../../../qtsmartgraphics/qtsmartgraphics/qsmartlabel.h \
    ../../libsources/qlabelbar.h \
    ../../lib/imageprocessing.h \
    ../../lib/imgcore.h \
    colorsplittertoolplugin.h
DISTFILES += colorSplitter.json

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += mainwindow.ui

msvc {
  QMAKE_CXXFLAGS += -openmp -arch:AVX -D "_CRT_SECURE_NO_WARNINGS"
  QMAKE_CXXFLAGS_RELEASE *= -O2
}

INCLUDEPATH += D:/Database/QtProject/qtsmartgraphics/qtsmartgraphics\
               $$quote(D:/libraries/opencv249o/include)\
               $$PWD\
               $$quote(../../lib)\
               $$quote(../../libsources)

win32{
    !contains(QMAKE_TARGET.arch, x86_64){
         message("x86_64")
         OPENCVLIB += $$quote(D:/libraries/opencv249o/x86/vc12/lib)
         TARGET = $$qtLibraryTarget(sip_colorSplitterTool_x86)
         # install
         target.path = ../../build-simpleProcess-Desktop_Qt_5_5_0_MSVC2013_32bit-Release/plugins
         INSTALLS += target
         DESTDIR = ../../build-simpleProcess-Desktop_Qt_5_5_0_MSVC2013_32bit-Release/plugins
    } else {
         message("x64 build")
         OPENCVLIB += $$quote(D:/libraries/opencv249o/x64/vc12/lib)
         TARGET = $$qtLibraryTarget(sip_colorSplitterTool_x64)
         # install
         target.path = ../../build-simpleProcess-Desktop_Qt_5_5_0_MSVC2013_64bit-Release/plugins
         INSTALLS += target
         DESTDIR = ../../build-simpleProcess-Desktop_Qt_5_5_0_MSVC2013_64bit-Release/plugins
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
    ../../libsources/simpleProcessing.rc
RC_FILE +=\
    ../../libsources/simpleProcessing.rc

DEFINES += HAVE_OPENCV
DEFINES += ISCREATEDASLIBRARY
