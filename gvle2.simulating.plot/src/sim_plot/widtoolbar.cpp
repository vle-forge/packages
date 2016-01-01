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
#include "sim_plot.h"

widToolbar::widToolbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widToolbar)
{
    ui->setupUi(this);

    ui->widSimStyle->setVisible(false);

    QObject::connect(ui->butSimColor,  SIGNAL(clicked()), this, SLOT(onToolColor()));
}

widToolbar::~widToolbar()
{
    delete ui;
}

void widToolbar::onToolColor()
{
    QTreeWidgetItem *item = ui->treeSimViews->currentItem();

    QColorDialog *colorDialog = new QColorDialog(this);
    colorDialog->setOptions(QColorDialog::DontUseNativeDialog);
    if (colorDialog->exec())
    {
        plotSignal *plot = 0;
        QVariant vType = item->data(0, Qt::UserRole+3);
        if (vType.isValid())
            plot = (plotSignal *)vType.value<void *>();
        QColor c = colorDialog->selectedColor();
        QString style = "border:1px solid;";
        style += QString("background-color: rgb(%1, %2, %3);")
            .arg(c.red()).arg(c.green()).arg(c.blue());
        if (plot)
        {
            plot->setColor(c);
            ui->butSimColor->setStyleSheet(style);

            emit updateSig(plot);
        }
    }
}

int  widToolbar::sigCount()
{
    return mPlotSignals.count();
}

void widToolbar::buildTree(vle::vpz::Vpz *vpz)
{
    vle::vpz::Observables curVpzObs;
    vle::vpz::Views       curVpzViews;
    vle::vpz::AtomicModelVector curVpzModels;

    curVpzViews  = vpz->project().experiment().views();
    curVpzObs    = vpz->project().experiment().views().observables();
    vpz->project().model().getAtomicModelList(curVpzModels);

    // Initiate the view/port tree by adding the View list
    vle::vpz::Views::iterator itb = curVpzViews.begin();
    vle::vpz::Views::iterator ite = curVpzViews.end();
    for ( ; itb != ite; itb++)
    {
        vle::vpz::View& vv = itb->second;
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, QString(vv.name().c_str()));
        item->setIcon(0, *(new QIcon(":/icon/resources/magnifier.png")));
        mViewsItems.append(item);
    }

    // Look into each Model used be current VPZ
    vle::vpz::AtomicModelVector::iterator model;
    for (model = curVpzModels.begin(); model != curVpzModels.end(); model++)
    {
        // Only model with Observable(s) are relevant
        if ((*model)->observables() == "")
            continue;

        const vle::vpz::Observable obs = curVpzObs.get((*model)->observables());
        const vle::vpz::ObservablePortList& portList = obs.observableportlist();

        // Look each port of the current model
        vle::vpz::ObservablePortList::const_iterator port;
        for (port = portList.begin(); port != portList.end(); port++)
        {
            const vle::vpz::ViewNameList& viewList = port->second.viewnamelist();
            // If the port isn't used for any view ... nothing to do
            if (viewList.begin() == viewList.end())
                continue;

            vle::vpz::ViewNameList::const_iterator view;
            for (view = viewList.begin(); view != viewList.end(); view++)
            {
                vle::vpz::View    selView = curVpzViews.get((*view));
                vle::vpz::Output& output(curVpzViews.outputs().get(selView.output()));
                QString pluginName = QString("%1/%2")
                        .arg(output.package().c_str())
                        .arg(output.plugin().c_str());
                if (pluginName == "vle.output/storage")
                {
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
    updateViews();
}

/**
 * @brief widToolbar::insertViewPort
 *        Add a model-port into the tree
 */
void widToolbar::insertViewPort(QString viewName, QString portName, QString portInfo)
{
    QListIterator<QTreeWidgetItem*> items( mViewsItems );

    while( items.hasNext() )
    {
        QTreeWidgetItem *item = items.next();
        if (item->text(0) == viewName)
        {
            QTreeWidgetItem *newItem = new QTreeWidgetItem();
            newItem->setText(0, portName);
            newItem->setFlags(newItem->flags()|Qt::ItemIsUserCheckable);
            newItem->setCheckState(0, Qt::Unchecked);
            newItem->setData(0, Qt::UserRole+0, "typePort");
            newItem->setData(0, Qt::UserRole+1, viewName);
            newItem->setData(0, Qt::UserRole+2, portName);
            newItem->setToolTip(0, portInfo);
            newItem->setIcon(0, *(new QIcon(":/icon/resources/comment.png")));
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
    QObject::connect(viewTree, SIGNAL(itemSelectionChanged ()),
                     this,     SLOT  (onTreeItemSelected()));
}

/**
 * @brief widToolbar::onTreeItemSelected
 *        Called when the currently selected item into view/port tree change
 *
 */
void widToolbar::onTreeItemSelected()
{
    QTreeWidgetItem *item = ui->treeSimViews->currentItem();
    bool itemVisility = false;

    if (item)
    {
        // Get the saved pointer on associated simPlot object
        QVariant vType = item->data(0, Qt::UserRole+3);
        if (vType.isValid())
        {
            plotSignal *plot = (plotSignal *)vType.value<void *>();

            QString style = "border:1px solid; ";
            if (plot)
            {
                QColor c = plot->getColor();
                style += QString("background-color: rgb(%1, %2, %3);")
                        .arg(c.red()).arg(c.green()).arg(c.blue());
            }

            int state = item->checkState(0);
            if (state == Qt::Checked)
            {
                ui->butSimColor->setStyleSheet(style);
                itemVisility = true;
            }
        }
    }
    ui->widSimStyle->setVisible(itemVisility);
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

    if (state == Qt::Checked)
    {
        if (qvPlot.isValid())
            return;

        // Create a new simPlot object
        plotSignal *newPlot = new plotSignal();
        newPlot->setSignal(viewName, portName);
        mPlotSignals.append(newPlot);

        // Save a pointer to the SimPlot into TreeWidget
        QVariant qvPtr = QVariant::fromValue((void *)newPlot);
        item->setData(0, Qt::UserRole+3, qvPtr);

        emit addSig(newPlot);

        if (ui->treeSimViews->currentItem() == item)
            onTreeItemSelected();
        else
            ui->treeSimViews->setCurrentItem(item);
    }
    else
    {
        // Get the saved pointer on associated simPlot object
        QVariant vPlot = item->data(0, Qt::UserRole+3);
        if (vPlot.isValid())
        {
            plotSignal *plot = (plotSignal *)vPlot.value<void *>();
            for (int k = 0; k < mPlotSignals.count(); k++)
            {
                plotSignal *savedPlot = mPlotSignals.at(k);
                if (savedPlot != plot)
                    continue;

                // Remove it from the list of used signels
                mPlotSignals.removeAt(k);
                // Notify that the signal will be deleted (must be Qt::DirectConnection)
                emit delSig(plot);
                // Then clean memory
                delete savedPlot;
            }

            // Remove the simPLot pointer from TreeWidget
            item->setData(0, Qt::UserRole+3, QVariant() );

            if (ui->treeSimViews->currentItem() == item)
                onTreeItemSelected();
            else
                ui->treeSimViews->setCurrentItem(item);
        }
    }
}

void widToolbar::simStarted()
{
    ui->treeSimViews->setEnabled(false);
}
void widToolbar::simPaused()
{
    // Nothing to do in toolbar when sim is paused
}

void widToolbar::simStoped()
{
    ui->treeSimViews->setEnabled(true);
}
