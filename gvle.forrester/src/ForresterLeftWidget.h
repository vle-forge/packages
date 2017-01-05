/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2015 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GVLE_FORRESTER_LEFT_WIDGET_H
#define GVLE_FORRESTER_LEFT_WIDGET_H

#include <QtCore/QVariant>
#include <QMenu>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <vle/gvle/vlevpz.hpp>

#include "vlesmForrester.h"

namespace Ui {
    class leftWidget;
}


namespace gvle {
namespace forrester {

class ForresterPanel;

/*************************/

class ForresterCompartment : public QGraphicsItem
{
public:
    ForresterCompartment(QDomNode node, vleSmForrester* smForrester);
    ~ForresterCompartment();
    int type() const;
    QRectF boundingRect() const;
    void paint(QPainter */*painter*/,
            const QStyleOptionGraphicsItem */*option*/,
            QWidget */*widget*/);
    void update(const QRectF & rect = QRectF());

    QString name() const;
    QGraphicsRectItem* getRectangle() const;
    QGraphicsTextItem* getNameItem() const;
    void setNameEdition(bool val);

    vleSmForrester* sm;
    QDomNode node;

};

/*************************/

class ForresterParameter : public QGraphicsItem
{
public:
    ForresterParameter(QDomNode node, vleSmForrester* smForrester);
    ~ForresterParameter();
    int type() const;
    QRectF boundingRect() const;
    void paint(QPainter */*painter*/,
            const QStyleOptionGraphicsItem */*option*/,
            QWidget */*widget*/);
    void update(const QRectF & rect = QRectF());

    QString name() const;
    QGraphicsRectItem* getRectangle() const;
    QGraphicsTextItem* getNameItem() const;
    void setNameEdition(bool val);


    vleSmForrester* sm;
    QDomNode node;

};

/*************************/

class ForresterMaterialFlow : public QGraphicsItem
{
public:
    ForresterMaterialFlow(QDomNode node, vleSmForrester* smForrester);
    ~ForresterMaterialFlow();
    int type() const;
    QRectF boundingRect() const;
    void paint(QPainter */*painter*/,
            const QStyleOptionGraphicsItem */*option*/,
            QWidget */*widget*/);
    void update(const QRectF & rect = QRectF());

    QGraphicsTextItem* getNameItem() const;
    void setNameEdition(bool val);

    QString name() const;

    vleSmForrester* sm;
    QDomNode node;

};

/*************************/

class ForresterSoftLink : public QGraphicsItem
{
public:
    ForresterSoftLink(QDomNode dep, ForresterCompartment* comp,
            ForresterMaterialFlow* flow);
    ForresterSoftLink(QDomNode dep, ForresterParameter* comp,
                ForresterMaterialFlow* flow);
    ~ForresterSoftLink();

    void clear();
    void reload();
    QGraphicsEllipseItem* getEllipse() const;
    QGraphicsItem* varItem() const;
    QGraphicsItem* matFlowItem() const;
    QString varName() const;
    QString matFlowName() const;

    int type() const;
    QRectF boundingRect() const;
    void paint(QPainter */*painter*/,
            const QStyleOptionGraphicsItem */*option*/,
            QWidget */*widget*/);
    void update(const QRectF & rect = QRectF());

    ForresterCompartment* comp;
    ForresterParameter* param;
    ForresterMaterialFlow* flow;


};

/*************************/

class ForresterHardLink : public QGraphicsLineItem
{
public:
    ForresterHardLink(ForresterMaterialFlow* _flow,
            ForresterCompartment* _comp);
    ForresterHardLink(ForresterCompartment* _comp,
            ForresterMaterialFlow* _flow);
    ~ForresterHardLink();

    int type() const override;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    void paint(QPainter */*painter*/,
            const QStyleOptionGraphicsItem */*option*/,
            QWidget */*widget*/);
    void update(const QRectF & rect = QRectF());

    ForresterMaterialFlow* flow;
    ForresterCompartment* comp;
    bool flow_to_comp;
    QPolygonF arrowHead;

};

/*****************************/

class ForresterScene : public QGraphicsScene
{
    Q_OBJECT
public:


    ForresterScene(ForresterPanel* p);



    void reload();
    void reloadSoftLinks();
    void reloadHardLinks();
    void reloadPath();
    void unselectAllItems();
    void removeNameEditionMode();
    void clearPath();
    void clearSoftLinks();
    void clearHardLinks();

    ForresterCompartment* itemCompartment(QString compName);
    ForresterParameter* itemParameter(QString paramName);
    ForresterMaterialFlow* itemMaterialFlow(QString matName);

    //QPointF centerQGraphicsItem(QGraphicsItem* item);

    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);
    void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void dragEnterEvent(QGraphicsSceneDragDropEvent * event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent * event);
    void dropEvent(QGraphicsSceneDragDropEvent * event);
public slots:
    void onDragDestroyed(QObject *obj =0);

public:
    ForresterPanel*  forrester;
};

class ForresterLeftWidget : public QWidget
{
    Q_OBJECT
public:
    ForresterLeftWidget(ForresterPanel*  f);
    virtual ~ForresterLeftWidget();

    void reload();

    void reloadGeneral();

    Ui::leftWidget* ui;
    ForresterScene* mScene;
    ForresterPanel*  forrester;
signals:
   //selection on the diagram
   void matFlowSelected(QString name);
   void compSelected(QString name);
   void paramSelected(QString name);
public slots:
   //when metadat is modified
   void onSmModified(int);

   //tab Forrester Diagramm, General, .. changed
   void onTabChanged(int);

   //reaction to numerical integration modifications
   void onNumMethodUpdate(int index);
   void tsChanged(double);
   void onCompUpdate(const QString& comp);
   void qtChanged(double);


};

}} //namespaces


#endif
