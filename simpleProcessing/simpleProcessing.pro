#-------------------------------------------------
#
# Project created by QtCreator 2014-10-16T20:09:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = simpleProcessing
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    lib/imageprocessing.cpp \
    smartgraphicsview.cpp \
    lib/imgcore.cpp \
    dialog/dialogresize.cpp \
    dialog/dialogblur.cpp \
    dialog/dialogpreference.cpp \
    smartlabel.cpp \
    dialog/dialogsize.cpp \
    lib/imageprocessing/fuzzylogic.cpp \
    qcustomplot.cpp \
    dialog/dialogfuzzylogic.cpp

HEADERS  += mainwindow.h \
    lib/imageprocessing.h \
    smartgraphicsview.h \
    lib/imgcore.h \
    dialog/dialogresize.h \
    dialog/dialogblur.h \
    dialog/dialogpreference.h \
    smartlabel.h \
    dialog/dialogsize.h \
    lib/imageprocessing/fuzzylogic.h \
    qcustomplot.h \
    dialog/dialogfuzzylogic.h

FORMS    += mainwindow.ui \
    dialog/dialogblur.ui \
    dialog/dialogpreference.ui \
    dialog/dialogresize.ui \
    dialog/dialogsize.ui \
    dialog/dialogfuzzylogic.ui

RESOURCES += \
    qdarkstyle/style.qrc \
    octicons/octicons.qrc \
    icon.qrc

msvc {
  QMAKE_CXXFLAGS += -openmp -arch:AVX -D "_CRT_SECURE_NO_WARNINGS"
  QMAKE_CXXFLAGS_RELEASE *= -O2
}

INCLUDEPATH += $$quote(D:/libraries/opencv249o/include)

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
    simpleProcessing.rc
RC_FILE +=\
    simpleProcessing.rc



