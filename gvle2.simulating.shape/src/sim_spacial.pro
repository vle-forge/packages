#
#  This file is part of VLE, a framework for multi-modeling, simulation
#  and analysis of complex dynamical systems.
#  http://www.vle-project.org
#
#  Copyright (c) 2014 INRA
#
QT       += core gui opengl

TEMPLATE = lib

TARGET = sim_spacial

CONFIG += plugin
INCLUDEPATH += ../
DESTDIR = ../
CONFIG += qtestlib

SOURCES += sim_spacial.cpp \
    simtab.cpp \
    widtoolbar.cpp \
    openglwidget.cpp \
    shapeobject.cpp \
    shapefile.cpp \
    shaderwidget.cpp
HEADERS += sim_spacial.h \
    simtab.h \
    widtoolbar.h \
    openglwidget.h \
    shapeobject.h \
    shapefile.h \
    shaderwidget.h

FORMS += \
    simtab.ui \
    widtoolbar.ui
