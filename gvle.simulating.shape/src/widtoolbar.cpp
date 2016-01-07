/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#include <QDebug>
#include <QTableWidgetItem>
#include "widtoolbar.h"
#include "ui_widtoolbar.h"

namespace vle {
namespace gvle {

/**
 * @brief widToolbar::widToolbar
 *        Default constructor for right column toolbar widget
 */
widToolbar::widToolbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widToolbar)
{
    mComboPalette = 0;

    ui->setupUi(this);

    // Configure properties tables
    QTableWidget *tProp;
    tProp = ui->tabProperties;
    tProp->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    // Configure config table
    tProp = ui->tabConfig;
    tProp->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    // Add a combobox into table for view selection
    mComboView = new QComboBox();
    ui->tabConfig->setCellWidget(ROW_VIEW, 1, mComboView);

    // Add a combobox into table for palette selection
    mComboPalette = new QComboBox();
    ui->tabConfig->setCellWidget(ROW_PALETTE, 1, mComboPalette);
    connect(mComboPalette, SIGNAL(currentIndexChanged(int)),
            this,          SLOT  (onPaletteSelected(int)));
}

/**
 * @brief widToolbar::~widToolbar
 *        Default destructor
 */
widToolbar::~widToolbar()
{
    delete ui;
}

/**
 * @brief widToolbar::setProperty
 *        Change the value of an entry into properties table
 */
void widToolbar::setProperty(int id, QString v)
{
    QTableWidgetItem *newItem = new QTableWidgetItem();
    newItem->setText(v);
    ui->tabProperties->setItem(id, 1, newItem);
}

/**
 * @brief widToolbar::addPalette
 *        Insert a palette into the property combobox
 */
void widToolbar::addPalette(QString name)
{
    mComboPalette->addItem(name);
}

/**
 * @brief widToolbar::onPaletteSelected
 *        Slot called when palette combobox selection change
 */
void widToolbar::onPaletteSelected(int n)
{
    // Re-emit signal for external use
    emit paletteSelected(n);
}

/**
 * @brief widToolbar::buildViewsList
 *        Load ths list of views from VPZ
 */
void widToolbar::buildViewsList(vleVpm *vpm)
{
    vle::vpz::Vpz   *oldVpz;
    vle::vpz::Views  curVpzViews;

    // NOTE - View list is loaded from disk using vle::Vpz
    // only because GVLE::vleVpz does not support views
    // direct access yet. This must be changed in future.

    QString fileName = vpm->getFilename();
    oldVpz = new vle::vpz::Vpz(fileName.toStdString());

    curVpzViews  = oldVpz->project().experiment().views();
    vle::vpz::Views::iterator itb = curVpzViews.begin();
    vle::vpz::Views::iterator ite = curVpzViews.end();
    for ( ; itb != ite; itb++)
    {
        vle::vpz::View& vv = itb->second;
        mComboView->addItem( QString(vv.name().c_str()) );
    }

    delete oldVpz;
}

/**
 * @brief widToolbar::getSelectedView
 *        Return the name of the currently selected view
 */
QString widToolbar::getSelectedView()
{
    return mComboView->currentText();
}

}}
