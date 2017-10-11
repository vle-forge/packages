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

#ifndef GVLE_DECISION_LEFT_WIDGET_H
#define GVLE_DECISION_LEFT_WIDGET_H

#include <vle/gvle/gvle_widgets.h>
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
#include <vle/gvle/gvle_widgets.h>

#include "vledmdd.h"

namespace Ui {
    class leftWidget;
}

namespace vle {
namespace gvle {

enum RuleTreeType {
    ENone,
    ERule,
    EPredicate
};

class DecisionPanel;

/*************************/

class DecisionActivity : public QGraphicsItem
{
public:
    DecisionActivity(QDomNode node, vleDmDD* dmDecision);
    ~DecisionActivity();
    int type() const;
    QRectF boundingRect() const;
    void paint(QPainter */*painter*/,
            const QStyleOptionGraphicsItem */*option*/,
            QWidget */*widget*/);
    void update(const QRectF & rect = QRectF());

    QString name() const;
    QGraphicsRectItem* getRectangle() const;
    VleLineEditItem* getNameItem() const;
    void setNameEdition(bool val);

    vleDmDD* dm;
    QDomNode node;

};


class DecisionPrecedence : public QGraphicsLineItem
{
public:
    DecisionPrecedence(DecisionActivity* _first,
            DecisionActivity* _second);
    ~DecisionPrecedence();

    int type() const override;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    void paint(QPainter */*painter*/,
            const QStyleOptionGraphicsItem */*option*/,
            QWidget */*widget*/);
    void update(const QRectF & rect = QRectF());
    QString firstName() const;
    QString secondName() const;

    DecisionActivity* first;
    DecisionActivity* second;
    QPolygonF arrowHead;

};

class DecisionScene : public QGraphicsScene
{
    Q_OBJECT
public:


    DecisionScene(DecisionPanel* p);



    void reload();
    void reloadPrecedences();
    void unselectAllItems();
    void removeNameEditionMode();
    void clearPath();
    void clearSoftLinks();
    void clearPrecedences();

    DecisionActivity* itemActivity(QString compName);

    QPointF centerQGraphicsItem(QGraphicsItem* item);

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
    DecisionPanel*  decision;
};

class DecisionLeftWidget : public QWidget
{
    Q_OBJECT
public:
    DecisionLeftWidget(DecisionPanel*  d);
    virtual ~DecisionLeftWidget();

    void reload();
    void reloadPredicates();
    void reloadRules();
    void reloadPredicates(const QString& ruleName,
		     QTreeWidgetItem* ruleItem);
    void insertRowPred(int row, const QString& name);
    VleLineEdit* getTextEdit(int row, int col);

    Ui::leftWidget* ui;
    DecisionScene* mScene;
    DecisionPanel* decision;
    QString mCurrPredicate;

signals:
   //selection on the diagram
   void actSelected(QString name);
   void precedenceSelected(QString first, QString last);
   void nothingSelected();
public slots:
   //when metadat is modified
   void onDmModified(int);
   void onOutputsType();
   void onItemChanged(QTreeWidgetItem *item,
		    int /*column*/);
   //tab Plan Diagramm, General, .. changed
   void onTabChanged(int);
   void onTablePredicatesMenu(const QPoint& pos);
   void onRulesMenu(const QPoint pos);
   void onPredNameUpdated(const QString& id,
			  const QString& oldname,
			  const QString& newname);

   void onSelected(const QString& id);
   void onOperatorUpdated(const QString& id, const QString& val);
   void onLeftTypeUpdated(const QString& id, const QString& val);
   void onRightTypeUpdated(const QString& id, const QString& val);
   void onRightNameUpdated(const QString& id,
			  const QString& oldname,
			  const QString& newname);
   void onLeftNameUpdated(const QString& id,
			  const QString& oldname,
			  const QString& newname);
   void onRightNumUpdated(const QString& id, double val);
   void onLeftNumUpdated(const QString& id, double val);

private:
    int itemType(const QTreeWidgetItem * item) const;
    QTreeWidgetItem* newItem(const RuleTreeType type, const QString name);
};

}} //namespaces


#endif
