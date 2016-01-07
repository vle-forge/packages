/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#include <QFileDialog>
#include <QDebug>
#include "simtab.h"
#include "ui_simtab.h"
#include "shaderwidget.h"
#include "vle/value/Map.hpp"
#include "vle/value/Value.hpp"
#include "vle/vpz/Views.hpp"
#include "vle/vpz/BaseModel.hpp"
#include "vle/vpz/AtomicModel.hpp"

namespace vle {
namespace gvle {

SimTab::SimTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SimTab)
{
    mSettings = 0;

    mVpm = 0;
    mOutputs = 0;

    mShapeFile = 0;
    mIsShape   = false;

    mThread = 0;

    mUiTool = 0;
    ui->setupUi(this);
    QObject::connect(ui->butStart, SIGNAL(clicked()),
                     this, SLOT(onButtonStart()));

    QObject::connect(ui->resultsSlider, SIGNAL(valueChanged(int)),
                     this, SLOT(onSliderValue(int)));

     QObject::connect(ui->buttonSave, SIGNAL(clicked()),
                      this,           SLOT(onButtonSave()));
}

SimTab::~SimTab()
{
    clearResults();

    delete ui;
}

/**
 * @brief SimTab::setToolbar
 *        Register the right column toolbar widget (when allocated by plugin main object)
 */
void SimTab::setToolbar(widToolbar *w)
{
    mUiTool = w;

    // Update the palette combobox according to the list of known palettes
    QList <shaderPalette *> *list;
    list = ui->widShader->getPaletteList();
    for (int i = 0; i < list->length(); i++)
        mUiTool->addPalette(list->at(i)->getName());
    connect(mUiTool, SIGNAL(paletteSelected(int)),
            this,    SLOT  (onPaletteSelected(int)));

    if (mVpm)
        mUiTool->buildViewsList(mVpm);
}

/**
 * @brief SimTab::setSettings
 *        Register a pointer to the QSettings (config file)
 */
void SimTab::setSettings(QSettings *s)
{
    mSettings = s;

    bool    keyValid = true;
    QString keyName;
    int  n = 0;
    while(keyValid)
    {
        keyName = QString("Plugin-Spacial/palette_%1").arg(n);
        QVariant vPalette = mSettings->value(keyName);
        if (vPalette.isValid())
        {
            n++;
            if (vPalette.type() != QVariant::String)
            {
                qWarning() << "Spacial: " << tr("malformed palette setting at index") << (n - 1);
                continue;
            }
            QString sPalette = vPalette.toString();

            ui->widShader->addPalette(sPalette);
        }
        else
            keyValid = false;
    }
}

void SimTab::setVpm(vleVpm *vpm)
{
    mVpm = vpm;

    if (mUiTool)
        mUiTool->buildViewsList(mVpm);
}
void SimTab::setPackage(vle::utils::Package *pkg)
{
    mCurrPackage = pkg;
}

void SimTab::setFile(ShapeFile *f)
{
    mShapeFile = f;
}

void SimTab::setShape()
{
    for (int i = 0; i < mShapeFile->getEntityCount(); i++)
    {
        ShapeObject *object = new ShapeObject();
        mShapeFile->getShape(i, object);
        ui->glview->addShape(object);
    }
}

void SimTab::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if (mShapeFile && (mIsShape == false))
    {
        setShape();
        mIsShape = true;
    }

}

void SimTab::onButtonStart()
{
    mSimThread = new simulationThread(mVpm);
    try {
        mCurrPackage->addDirectory("","output",vle::utils::PKG_SOURCE);
        std::string path = mCurrPackage->getOutputDir(vle::utils::PKG_SOURCE);
        mSimThread->setOutputPath(path.c_str());
        mSimThread->load();
        mSimThread->init();
        mOutputs = mSimThread->getOutputs();
    } catch(...) {
        qDebug() << "SimTab::onButtonStart() Error : " << mSimThread->getError();
        delete mSimThread;
        mSimThread = 0;
        return;
    }

    clearResults();

    mThread = new QThread();
    mSimThread->moveToThread(mThread);
    connect(mThread,    SIGNAL(started()), mSimThread, SLOT(run()));
    connect(mSimThread, SIGNAL(step()),    this, SLOT(simulationGetStep()));
    connect(mSimThread, SIGNAL(end()),     this, SLOT(simulationFinished()));
    mThread->start();
}

/**
 * @brief simulationView::onButtonSave
 *        Called when a click event occur on 'Save' button
 *
 */
void SimTab::onButtonSave()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save as"), tr("Choose a filename"), "PNG(*.png)");

    if (fileName != "")
    {
        try {
            QPixmap img = QPixmap::grabWidget(ui->glview);

            if(!img.save(fileName, qPrintable("PNG")))
                throw(2);
        } catch (...) {
            qDebug() << tr("Error: Save plot to image file failed");
        }
    }
}

void SimTab::onSliderValue(int val)
{
    if ((val + 1) > mSimResults.length())
        return;

    mUiTool->setProperty(3, QString("%1 / %2").arg(val).arg(mSimResults.length()));

    SimValue *v = mSimResults.at(val-1);

    for (int i = 0; i < v->polygonValues.length(); i++)
    {
        double vD = v->polygonValues.at(i);

        // Translate the value to range [0.0 : 1.0]
        double fullScale = mScaleMax - mScaleMin;
        double vRatio = (1 / fullScale) * (vD - mScaleMin);
        // Get the color for this value into current palette
        QColor c = ui->widShader->getColor(vRatio);
        // Update the shape color
        ui->glview->setObjectColor(i, c);
    }
    ui->glview->updateGL();
}

void SimTab::onPaletteSelected(int n)
{
    // Change the palette
    ui->widShader->setPalette(n);

    if (mSimResults.length())
    {
        // Refresh using slider
        onSliderValue(ui->resultsSlider->value());
    }
}

void SimTab::simulationGetStep()
{
    double duration = mSimThread->getDuration();
    double expTime = mSimThread->getCurrentTime();
    double elapsed = expTime - mSimThread->getStartTime();
    double percent = (elapsed / duration) * 100.00;

    ui->progressBar->setFormat("%p%");
    ui->progressBar->setValue(percent);
}

void SimTab::simulationFinished()
{
    if (mOutputs == 0)
        return;

    QString selectedViewName = mUiTool->getSelectedView();

    try {
        vle::value::Map::iterator itb = mOutputs->begin();
        vle::value::Map::iterator ite = mOutputs->end();
        for (; itb != ite; itb++)
        {
            // Get the view name
            QString matViewName = QString::fromStdString(itb->first);
            // If this is -not- the selected view, ignore it
            if (matViewName != selectedViewName)
                continue;

            vle::value::Matrix* mat;
            mat = mSimThread->getMatrix(itb->second);
            if (mat->rows() == 0)
            {
                delete mat;
                continue;
            }

            for (unsigned int i = 0; i < mat->rows(); i++)
            {
                SimValue *sv = new SimValue();
                for (unsigned int j = 1; j < mat->columns(); j++)
                {
                    vle::value::Value *v = mat->get(j, i);
                    if (v->getType() != 2)
                        continue;
                    double vDouble = mat->getDouble(j, i);

                    sv->polygonValues.append(vDouble);

                    if (! mScaleValid)
                    {
                        mScaleMin = vDouble;
                        mScaleMax = vDouble;
                        mScaleValid = true;
                        continue;
                    }
                    if (vDouble < mScaleMin)
                        mScaleMin = vDouble;
                    if (vDouble > mScaleMax)
                        mScaleMax = vDouble;
                }
                mSimResults.append(sv);
            }

            break;
        }
    } catch(...) {
        //
    }

    //
    mUiTool->setProperty(0, QString("%1").arg(mSimResults.length()));
    mUiTool->setProperty(1, QString("%1").arg(mScaleMin));
    mUiTool->setProperty(2, QString("%1").arg(mScaleMax));
    //
    ui->resultsSlider->setMinimum(1);
    ui->resultsSlider->setMaximum(mSimResults.length());
    // Show results slider (and hide simulation bar)
    ui->stk->setCurrentIndex(1);

    try {
        // Call finish to close vle::devs job
        mSimThread->finish();
        // Terminate simulation thread
        mThread->quit();
        mThread->wait();
        delete mThread;
        mThread = 0;
        // Clean memory
        delete mSimThread;
        mSimThread = 0;
    } catch (...) {
        qDebug() << mSimThread->getError();
    }
}

void SimTab::clearResults()
{
    while(mSimResults.length())
        delete mSimResults.takeFirst();

    mScaleValid = false;
}

}}
