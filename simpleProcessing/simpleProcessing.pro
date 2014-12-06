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
    ../lib/imageprocessing.cpp \
    ../lib/imgcore.cpp \
    ../lib/imageprocessing/fft.cpp \
    ../lib/imageprocessing/fuzzylogic.cpp \
    ../lib/imageprocessing/spectralfiltering.cpp \
    ../libsources/qcustomplot.cpp \
    ../libsources/smartgraphicsview.cpp \
    ../libsources/smartlabel.cpp \
    ../libsources/localsocketipc.cpp\
    ../libsources/sharememory.cpp \
    ../libsources/checkPID.cpp

HEADERS  += mainwindow.h \
    dialog/dialogresize.h \
    dialog/dialogblur.h \
    dialog/dialogpreference.h \
    dialog/dialogsize.h \
    dialog/dialogfuzzylogic.h \
    tool/dialogcustomfilter.h \
    tool/dialogimagesubtracttool.h \
    ../lib/imageprocessing.h \
    ../lib/imgcore.h \
    ../lib/imageprocessing/fft.h \
    ../lib/imageprocessing/fuzzylogic.h \
    ../lib/imageprocessing/spectralfiltering.h \
    ../libsources/qcustomplot.h \
    ../libsources/smartgraphicsview.h \
    ../libsources/smartlabel.h \
    ../libsources/localsocketipc.h \
    ../libsources/sharememory.h \
    ../libsources/checkPID.h

FORMS    += mainwindow.ui \
    dialog/dialogblur.ui \
    dialog/dialogpreference.ui \
    dialog/dialogresize.ui \
    dialog/dialogsize.ui \
    dialog/dialogfuzzylogic.ui \
    tool/dialogcustomfilter.ui \
    tool/dialogimagesubtracttool.ui \

RESOURCES += \
    ../libsources/octicons/octicons.qrc \
    ../libsources/qdarkstyle/style.qrc \
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

