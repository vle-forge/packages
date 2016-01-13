/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef WIDTOOLBAR_H
#define WIDTOOLBAR_H

#include <QList>
#include <QWidget>
#include <QTreeWidgetItem>
#include <vle/vpz/Vpz.hpp>
#include "vle/vpz/AtomicModel.hpp"

namespace Ui {
class widToolbar;
}

class widToolbar : public QWidget
{
    Q_OBJECT

public:
    explicit widToolbar(QWidget *parent = 0);
    ~widToolbar();
    void buildTree(vle::vpz::Vpz *vpz);
    int  sigCount();
    void simStarted();
    void simPaused();
    void simStoped();

public slots:
    void onTreeItemChanged(QTreeWidgetItem * item, int column);

signals:

    void addSig(QString, QString);
    void delSig(QString);

private:
    void insertViewPort(QString viewName, QString portName, QString portInfo);
    void updateViews();

private:
    Ui::widToolbar *ui;
    QList<QTreeWidgetItem *> mViewsItems;
    QMultiMap<QString, QString> mSelection;
    QMultiMap<QString, QString> mSelectionCleaner;
};

#endif // WIDTOOLBAR_H
