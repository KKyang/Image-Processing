#-------------------------------------------------
#
# Project created by QtCreator 2016-06-18T10:34:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = $$qtLibraryTarget(sip_colorSplitter)
TEMPLATE = lib
CONFIG += plugin

# install
target.path = ../../build-simpleProcessNewGUI-Desktop_Qt_5_5_0_MSVC2013_64bit-Release/plugins
INSTALLS += target
DESTDIR = ../../build-simpleProcessNewGUI-Desktop_Qt_5_5_0_MSVC2013_64bit-Release/plugins

SOURCES += colorsplitterplugin.cpp \
    mainwindow.cpp

HEADERS += colorsplitterplugin.h \
    mainwindow.h
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
    ../../libsources/simpleProcessing.rc
RC_FILE +=\
    ../../libsources/simpleProcessing.rc

DEFINES += HAVE_OPENCV
