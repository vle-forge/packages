/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#include <QDebug>
#include <QColorDialog>
#include "widtoolbar.h"
#include "ui_widtoolbar.h"
#include "sim_log.h"

widToolbar::widToolbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widToolbar)
{
    ui->setupUi(this);
}

widToolbar::~widToolbar()
{
    delete ui;
}

int  widToolbar::sigCount()
{
    return 0;
}

void widToolbar::buildTree(vle::vpz::Vpz *vpz)
{
    mViewsItems.clear();

    vle::vpz::Observables curVpzObs;
    vle::vpz::Views curVpzViews;
    vle::vpz::AtomicModelVector curVpzModels;

    curVpzViews = vpz->project().experiment().views();
    curVpzObs = vpz->project().experiment().views().observables();
    vpz->project().model().getAtomicModelList(curVpzModels);

    // Initiate the view/port tree by adding the View list
    vle::vpz::Views::iterator itb = curVpzViews.begin();
    vle::vpz::Views::iterator ite = curVpzViews.end();
    for ( ; itb != ite; itb++) {
        vle::vpz::View& vv = itb->second;
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, QString(vv.name().c_str()));
        item->setIcon(0, *(new QIcon(":/icon/resources/icon/magnifier.png")));
        mViewsItems.append(item);
    }

    // Look into each Model used be current VPZ
    vle::vpz::AtomicModelVector::iterator model;
    for (model = curVpzModels.begin(); model != curVpzModels.end(); model++) {
        // Only model with Observable(s) are relevant
        if ((*model)->observables() == "")
            continue;

        const vle::vpz::Observable obs = curVpzObs.get((*model)->observables());
        const vle::vpz::ObservablePortList& portList = obs.observableportlist();

        // Look each port of the current model
        vle::vpz::ObservablePortList::const_iterator port;
        for (port = portList.begin(); port != portList.end(); port++) {
            const vle::vpz::ViewNameList& viewList = port->second.viewnamelist();
            // If the port isn't used for any view ... nothing to do
            if (viewList.begin() == viewList.end())
                continue;

            vle::vpz::ViewNameList::const_iterator view;
            for (view = viewList.begin(); view != viewList.end(); view++) {
                vle::vpz::View    selView = curVpzViews.get((*view));
                vle::vpz::Output& output(curVpzViews.outputs().get(selView.output()));
                QString pluginName = QString("%1/%2")
                        .arg(output.package().c_str())
                        .arg(output.plugin().c_str());
                if (pluginName == "vle.output/storage") {
                    QString portViewName = QString("%1.%2")
                            .arg((*model)->getName().c_str())
                            .arg(QString::fromStdString(port->first));
                    QString portModelPath((*model)->getParentName().c_str());
                    portModelPath.replace(QChar(','), QString(" / "));
                    QString portFullName = QString("%1:%2")
                            .arg(portModelPath)
                            .arg(portViewName);
                    insertViewPort(QString::fromStdString(*view), portViewName, portFullName);
                }
            }
        }
    }
    QMultiMap<QString, QString>::iterator it;
    for(it = mSelection.begin(); it != mSelection.end(); ++it) {
        if (not mSelectionCleaner.contains(it.key(), it.value())) {
            it = mSelection.erase(it);
        }
    }
    mSelectionCleaner.clear();
    updateViews();
}

void widToolbar::insertViewPort(QString viewName, QString portName, QString portInfo)
{
    QListIterator<QTreeWidgetItem*> items( mViewsItems );

    while( items.hasNext() ) {
        QTreeWidgetItem *item = items.next();
        if (item->text(0) == viewName) {
            QTreeWidgetItem *newItem = new QTreeWidgetItem();
            newItem->setText(0, portName);
            newItem->setFlags(newItem->flags()|Qt::ItemIsUserCheckable);
            if (mSelection.contains(viewName, portName)) {
                newItem->setCheckState(0, Qt::Checked);
                mSelectionCleaner.insert(viewName, portName);
            } else {
                newItem->setCheckState(0, Qt::Unchecked);
            }
            newItem->setData(0, Qt::UserRole+0, "typePort");
            newItem->setData(0, Qt::UserRole+1, viewName);
            newItem->setData(0, Qt::UserRole+2, portName);
            newItem->setToolTip(0, portInfo);
            newItem->setIcon(0, *(new QIcon(":/icon/resources/icon/comment.png")));
            item->addChild(newItem);
            break;
        }
    }
}

/**
 * @brief widToolbar::updateViews
 *        Insert the model tree into toolbox widget
 */
void widToolbar::updateViews()
{
    QTreeWidget *viewTree = ui->treeSimViews;

    viewTree->clear();
    viewTree->setColumnCount(1);
    viewTree->setHeaderLabels(QStringList("Views"));
    if (mViewsItems.count())
        viewTree->insertTopLevelItems(0, mViewsItems);

    QObject::connect(viewTree, SIGNAL(itemChanged      (QTreeWidgetItem *, int)),
                     this,     SLOT  (onTreeItemChanged(QTreeWidgetItem *, int)));
}

/**
 * @brief widToolbar::onTreeItemChanged
 *        Called when an item into view/port tree is checked or unchecked
 *
 */
void widToolbar::onTreeItemChanged(QTreeWidgetItem *item, int column)
{
    int state = item->checkState(column);
    QString viewName = item->data(0, Qt::UserRole+1).toString();
    QString portName = item->data(0, Qt::UserRole+2).toString();
    QVariant qvPlot  = item->data(0, Qt::UserRole+3);

    if (state == Qt::Checked) {
        if (qvPlot.isValid()) {
            return;
        }
        emit addSig(viewName, portName);
        mSelection.insert(viewName, portName);
    } else {
        // Get the saved pointer on associated simPlot object
        QVariant vPlot = item->data(0, Qt::UserRole+3);
        if (vPlot.isValid()) {
            // Remove the simPLot pointer from TreeWidget
            item->setData(0, Qt::UserRole+3, QVariant() );
        }

        emit delSig(portName);
        mSelection.remove(viewName, portName);
    }
}

void widToolbar::simStarted()
{
    ui->treeSimViews->setEnabled(false);
}
void widToolbar::simPaused()
{
}

void widToolbar::simStoped()
{
    ui->treeSimViews->setEnabled(true);
}
