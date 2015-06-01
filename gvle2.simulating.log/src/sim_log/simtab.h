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

#include <QWidget>
#include <QPlainTextEdit>

namespace Ui {
class SimTabLog;
}

class SimTabLog : public QWidget
{
    Q_OBJECT

public:
    explicit SimTabLog(QWidget *parent = 0);
    ~SimTabLog();
    void setModelName(QString name);
    QPlainTextEdit *getPlotWidget();
    void simStarted();
    void simPaused();
    void simStoped();
    void progressBar(int value);
    void progressBar(QString format);

signals:
     void doStartStop();

private slots:
    void onButtonGo();
    void onButtonSave();

private:
    Ui::SimTabLog *ui;
};

#endif // SIMTAB_H
