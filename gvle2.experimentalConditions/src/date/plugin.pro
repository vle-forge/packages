#
#  This file is part of VLE, a framework for multi-modeling, simulation
#  and analysis of complex dynamical systems.
#  http://www.vle-project.org
#
#  Copyright (c) 2015 INRA
#
QT       += core gui

TEMPLATE = lib

TARGET = expcond_dummy

CONFIG += plugin
INCLUDEPATH += ../../
DESTDIR = ../
CONFIG += qtestlib

SOURCES += plugin.cpp \
    tab.cpp \
    toolbar.cpp
HEADERS += plugin.h \
    tab.h \
    toolbar.h

FORMS += \
    tab.ui \
    toolbar.ui \
    widgetTab.ui
