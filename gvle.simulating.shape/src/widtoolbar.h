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

#include <QComboBox>
#include <QObject>
#include <QWidget>
#include <vle/gvle/vlevpm.h>
#ifndef Q_MOC_RUN
#include <vle/vpz/Vpz.hpp>
#endif

namespace Ui {
class widToolbar;
}

#define ROW_VIEW    0
#define ROW_PALETTE 1

namespace vle {
namespace gvle {

class widToolbar : public QWidget
{
    Q_OBJECT

public:
    explicit widToolbar(QWidget *parent = 0);
    ~widToolbar();
    void setProperty(int id, QString v);
    void addPalette(QString name);
    void buildViewsList(vleVpm *vpm);
    QString getSelectedView();

private:
    Ui::widToolbar *ui;
    QComboBox      *mComboPalette;
    QComboBox      *mComboView;
signals:
    void paletteSelected(int);
private slots:
    void onPaletteSelected(int);
};

}}

#endif // WIDTOOLBAR_H
