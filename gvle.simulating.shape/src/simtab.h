/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef SIMTAB_H
#define SIMTAB_H

#include <QObject>
#include <QSettings>
#include <QThread>
#include <QWidget>
#include <vle/gvle/vlevpz.h>
#include <vle/gvle/simulation.h>
#include "shapefile.h"
#include "widtoolbar.h"

#ifndef Q_MOC_RUN
#include <vle/utils/Package.hpp>
#include <vle/utils/ModuleManager.hpp>
#endif

namespace Ui {
class SimTab;
}

namespace vle {
namespace gvle {

class SimValue
{
public:
    QList <double> polygonValues;
};

class SimTab : public QWidget
{
    Q_OBJECT

public:
    explicit SimTab(QWidget *parent = 0);
    ~SimTab();
    void setToolbar(widToolbar *w);
    void setSettings(QSettings *s);
    void setVpm(vleVpm *vpm);
    void setPackage(vle::utils::Package *pkg);
    void setFile(ShapeFile *f);
    void setShape();

protected:
    void paintEvent(QPaintEvent *event);
    void clearResults();

private:
    Ui::SimTab *ui;
    widToolbar *mUiTool;
    QSettings  *mSettings;
    vleVpm     *mVpm;
    ShapeFile  *mShapeFile;
    bool        mIsShape;

    bool        mScaleValid;
    double      mScaleMin;
    double      mScaleMax;

    QList <SimValue *>   mSimResults;

    QThread             *mThread;
    simulationThread    *mSimThread;
    vle::utils::Package *mCurrPackage;
    vle::value::Map     *mOutputs;

public slots:
     void simulationGetStep();
     void simulationFinished();
private slots:
    void onButtonStart();
    void onSliderValue(int val);
    void onPaletteSelected(int n);
    void onButtonSave();
};

}}

#endif // SIMTAB_H
