/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#include <QDebug>
#include <QFileDialog>
#include "simtab.h"
#include "ui_simtab.h"

SimTabLog::SimTabLog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SimTabLog)
{
    ui->setupUi(this);

    QObject::connect(ui->buttonGo,   SIGNAL(clicked()),
                     this,           SLOT(onButtonGo()));
    QObject::connect(ui->buttonSave, SIGNAL(clicked()),
                     this,           SLOT(onButtonSave()));
}

SimTabLog::~SimTabLog()
{
    delete ui;
}

void SimTabLog::setModelName(QString name)
{
    ui->modelName->setText(name);
}

QPlainTextEdit *SimTabLog::getPlotWidget()
{
    return ui->log;
}

void SimTabLog::onButtonGo()
{
    emit doStartStop();
}

/**
 * @brief simulationView::onButtonSave
 *        Called when a click event occur on 'Save' button
 *
 */
void SimTabLog::onButtonSave()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save as"), tr("Choose a filename"), "TXT(*.txt)");

    if (fileName != "")
    {
        // try {
        //     QPixmap img = QPixmap::grabWidget(ui->qwtPlot);

        //     if(!img.save(fileName, qPrintable("PNG")))
        //         throw(2);
        // } catch (...) {
        //     qDebug() << tr("Error: Save plot to image file failed");
        // }
    }
}

void SimTabLog::simStarted()
{
    ui->buttonSave->setEnabled(false);
    ui->progressBar->setFormat("%p%");
    ui->progressBar->setTextVisible(true);
    ui->buttonGo->setIcon(*(new QIcon(":/icon/resources/icon/control_pause_blue.png")));
}
void SimTabLog::simPaused()
{
    ui->progressBar->setFormat(tr("%p% (Paused)"));
    ui->buttonGo->setIcon(*(new QIcon(":/icon/resources/icon/control_play_blue.png")));
}

void SimTabLog::simStoped()
{
    ui->buttonSave->setEnabled(true);
    progressBar("%p%");
    ui->buttonGo->setIcon(*(new QIcon(":/icon/resources/icon/control_play_blue.png")));
}

void SimTabLog::progressBar(int value)
{
    ui->progressBar->setValue(value);
}

void SimTabLog::progressBar(QString format = "")
{
    ui->progressBar->setFormat(format);
}
