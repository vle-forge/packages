#
#  This file is part of VLE, a framework for multi-modeling, simulation
#  and analysis of complex dynamical systems.
#  http://www.vle-project.org
#
#  Copyright (c) 2014 INRA
#
QT       += core gui

TEMPLATE = lib

TARGET = sim_plot

CONFIG += plugin
INCLUDEPATH += ../../
DESTDIR = ../
CONFIG += qtestlib

INCLUDEPATH += /opt/vle-1.c/include/vle-1.1/  \
               /opt/qwt-6.0.0/include/    \
               /usr/include/glib-2.0      \
               /usr/lib/glib-2.0/include/ \
               /usr/include/libxml2/
LIBS += -L/opt/vle-1.c/lib -lvle-1.1 -L/opt/qwt-6.0.0/lib -lqwt

SOURCES += sim_plot.cpp \
    simtab.cpp \
    widtoolbar.cpp \
    plotsignal.cpp \
    thread.cpp
HEADERS += sim_plot.h \
    simtab.h \
    widtoolbar.h \
    plotsignal.h \
    thread.h

FORMS += \
    simtab.ui \
    widtoolbar.ui
