#-------------------------------------------------
#
# Project created by QtCreator 2019-10-24T10:29:02
#
#-------------------------------------------------

QT       += core gui svg


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = drawApp002
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        basetool.cpp \
        filter.cpp \
        graphicsscene.cpp \
        graphicsview.cpp \
        handleitem.cpp \
        main.cpp \
        mainwindow.cpp \
        potrace/curve.c \
        potrace/decompose.c \
        potrace/potracelib.c \
        potrace/svg.c \
        potrace/trace.c \
        selectionrectitem.cpp \
        svgitem.cpp \
        svgtool.cpp \
        toGcode/GcodeGenerator.cpp \
        toGcode/SVGParser.cpp \
        toGcode/bezierlib/Arc.cpp \
        toGcode/bezierlib/BezierToBiarc.cpp \
        toGcode/bezierlib/BiArc.cpp \
        toGcode/bezierlib/CubicBezier.cpp \
        toGcode/bezierlib/Line.cpp \
        toGcode/gcode.cpp \
        tracer.cpp \
        util.cpp

HEADERS += \
        basetool.h \
        filter.h \
        graphicsscene.h \
        graphicsview.h \
        handleitem.h \
        mainwindow.h \
        potrace/auxiliary.h \
        potrace/bitmap.h \
        potrace/curve.h \
        potrace/decompose.h \
        potrace/lists.h \
        potrace/potracelib.h \
        potrace/progress.h \
        potrace/svg.h \
        potrace/trace.h \
        selectionrectitem.h \
        svgitem.h \
        svgtool.h \
        toGcode/GcodeGenerator.h \
        toGcode/SVGParser.h \
        toGcode/bezierlib/Arc.h \
        toGcode/bezierlib/BezierToBiarc.h \
        toGcode/bezierlib/BiArc.h \
        toGcode/bezierlib/CubicBezier.h \
        toGcode/bezierlib/Line.h \
        toGcode/dpoint.h \
        toGcode/gcode.h \
        toGcode/togcode.h \
        trace/bitmap.h \
        trace/filterset.h \
        trace/potrace/auxiliary.h \
        trace/potrace/bitmap.h \
        trace/potrace/curve.h \
        trace/potrace/decompose.h \
        trace/potrace/lists.h \
        trace/potrace/potracelib.h \
        trace/potrace/progress.h \
        trace/potrace/svg.h \
        trace/potrace/trace.h \
        trace/potrace_tracingengine.h \
        trace/trace.h \
        tracer.h \
        util.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

DISTFILES += \
    trace/potrace/readme.txt
