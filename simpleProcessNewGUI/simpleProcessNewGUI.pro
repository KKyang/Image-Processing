#-------------------------------------------------
#
# Project created by QtCreator 2016-06-16T17:55:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = simpleProcessNewGUI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    subwindowwidget.cpp \
    ../../qtsmartgraphics/qtsmartgraphics/qsmartgraphicsview.cpp

HEADERS  += mainwindow.h \
    subwindowwidget.h \
    ../../qtsmartgraphics/qtsmartgraphics/qsmartgraphicsview.h \
    plugininterfaces.h

FORMS    += mainwindow.ui \
    subwindowwidget.ui

RESOURCES += \
    ../libsources/octicons/octicons.qrc \
    ../libsources/qdarkstyle/style.qrc \
    ../libsources/icon.qrc

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
