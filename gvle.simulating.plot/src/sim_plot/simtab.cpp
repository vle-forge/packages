/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#include <QFileDialog>
#include "simtab.h"
#include "ui_simtab.h"

SimTab::SimTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SimTab)
{
    ui->setupUi(this);

    QObject::connect(ui->buttonGo,   SIGNAL(clicked()),
                     this,           SLOT(onButtonGo()));
    QObject::connect(ui->buttonSave, SIGNAL(clicked()),
                     this,           SLOT(onButtonSave()));
}

SimTab::~SimTab()
{
    delete ui;
}

void SimTab::setModelName(QString name)
{
    ui->modelName->setText(name);
}

QwtPlot *SimTab::getPlotWidget()
{
    return ui->qwtPlot;
}

void SimTab::updatePlot()
{
    ui->qwtPlot->replot();
}

void SimTab::onButtonGo()
{
    //
    emit doStartStop();
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
            QPixmap img = QPixmap::grabWidget(ui->qwtPlot);

            if(!img.save(fileName, qPrintable("PNG")))
                throw(2);
        } catch (...) {
            qDebug() << tr("Error: Save plot to image file failed");
        }
    }
}

void SimTab::simStarted()
{
    ui->buttonSave->setEnabled(false);
    ui->progressBar->setFormat("%p%");
    ui->progressBar->setTextVisible(true);
    ui->buttonGo->setIcon(*(new QIcon(":/icon/resources/control_pause_blue.png")));
}
void SimTab::simPaused()
{
    ui->progressBar->setFormat(tr("%p% (Paused)"));
    ui->buttonGo->setIcon(*(new QIcon(":/icon/resources/control_play_blue.png")));
}

void SimTab::simStoped()
{
    ui->buttonSave->setEnabled(true);
    progressBar("%p%");
    ui->buttonGo->setIcon(*(new QIcon(":/icon/resources/control_play_blue.png")));
}

void SimTab::progressBar(int value)
{
    ui->progressBar->setValue(value);
}

void SimTab::progressBar(QString format = "")
{
    ui->progressBar->setFormat(format);
}
