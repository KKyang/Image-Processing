#-------------------------------------------------
#
# Project created by QtCreator 2014-10-16T20:09:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = imageProcessing
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    lib/imageprocessing.cpp \
    smartgraphicsview.cpp \
    lib/imgcore.cpp \
    ptzlabel.cpp

HEADERS  += mainwindow.h \
    lib/imageprocessing.h \
    smartgraphicsview.h \
    lib/imgcore.h \
    ptzlabel.h

FORMS    += mainwindow.ui

RESOURCES += \
    qdarkstyle/style.qrc \
    octicons/octicons.qrc

msvc {
  QMAKE_CXXFLAGS += -openmp -arch:AVX -D "_CRT_SECURE_NO_WARNINGS"
  QMAKE_CXXFLAGS_RELEASE *= -O2
}

INCLUDEPATH += $$quote(D:/libraries/opencv249/include)

OPENCVLIB += $$quote(D:/libraries/opencv249/x64/vc12/lib)

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
