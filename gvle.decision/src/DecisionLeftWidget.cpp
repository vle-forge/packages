/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2017 INRA http://www.inra.fr
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

#include <iostream>
#include <cmath>

#include <QGraphicsSceneMouseEvent>
#include <QDrag>
#include <QMimeData>

#include "DecisionPanel.h"
#include "DecisionLeftWidget.h"
#include "ui_leftWidget.h"

namespace vle {
namespace gvle {

using namespace vle::gvle;

// /********************** Not in API *******************************/

enum {
     Activity       = QGraphicsItem::UserType + 1,
     PrecedencePath = QGraphicsItem::UserType + 2,
};

QPointF
intersection(QGraphicsItem* item, QPointF orig)
{
    QRectF itemRect(item->pos(),
            QPointF(item->pos().x() + item->boundingRect().bottomRight().x(),
                    item->pos().y() + item->boundingRect().bottomRight().y()));
    QLineF origToCenter(itemRect.center(), orig);
    QPointF inter(0,0);

    QLineF try_line (itemRect.topLeft(), itemRect.topRight());
    if (origToCenter.intersect(try_line, &inter) ==
            QLineF::BoundedIntersection) {
        return inter;
    }
    try_line = QLineF(itemRect.topRight(), itemRect.bottomRight());
    if (origToCenter.intersect(try_line, &inter) ==
            QLineF::BoundedIntersection) {
        return inter;
    }
    try_line = QLineF(itemRect.bottomRight(), itemRect.bottomLeft());
    if (origToCenter.intersect(try_line, &inter) ==
            QLineF::BoundedIntersection) {
        return inter;
    }
    try_line = QLineF(itemRect.bottomLeft(), itemRect.topLeft());
    if (origToCenter.intersect(try_line, &inter) ==
            QLineF::BoundedIntersection) {
        return inter;
    }
    return orig;
}

QColor
colorObj(bool selected)
{
    if (selected) {
        return QColor(255, 0, 0);
    } else {
        return QColor(0, 0, 255);
    }
}

QPen penContourObj(bool selected)
{
    return QPen(colorObj(selected), 3, Qt::SolidLine);
}

QPen penParameter(bool selected)
{
    return QPen(colorObj(selected), 2, Qt::DashDotLine);
}

QPen penSoftLinkPath()
{
    return QPen(QColor(0, 0, 0), 1.5, Qt::DashLine);
}

QPen penPrecedencePath(bool selected)
{
    return QPen(colorObj(selected), 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
}

QGraphicsLineItem* newPrecedencePath(QPointF orig, QPointF dest)
{
    QGraphicsLineItem* line = new QGraphicsLineItem(
        QLineF(orig, dest));
    line->setData(0, QVariant((int) PrecedencePath));
    line->setPen(penPrecedencePath(false));
    return line;
}

QPointF
centerQGraphicsItem(QGraphicsItem* item)
{
    float x = item->pos().x()+item->boundingRect().x()
            + item->boundingRect().width()/2.0;

    float y = item->pos().y()+item->boundingRect().y()
            + item->boundingRect().height()/2.0;
    return QPointF(x,y);
}


//********************** DecisionActivity *******************************/

DecisionActivity::DecisionActivity(QDomNode n,
        vleDmDD* dmDecision) : dm(dmDecision), node(n)
{
    VleLineEditItem* nameItem = new VleLineEditItem(
            DomFunctions::attributeValue(node, "name"), this);
    nameItem->setFlag(QGraphicsItem::ItemIsSelectable, false);
    QGraphicsItem::setFlag(QGraphicsItem::ItemIsMovable, false);
    QGraphicsItem::setFlag(QGraphicsItem::ItemIsSelectable, true);
    update();
}

DecisionActivity::~DecisionActivity()
{
}

int
DecisionActivity::type() const
{
     return Activity;
}


QRectF
DecisionActivity::boundingRect() const
{
    return childrenBoundingRect();
}
void
DecisionActivity::paint(QPainter */*painter*/,
        const QStyleOptionGraphicsItem */*option*/,
        QWidget */*widget*/)
{
}

void
DecisionActivity::update(const QRectF & /*rect*/)
{
    QList<QGraphicsItem *>  children = childItems();
    for (int i =0; i<children.length(); i++) {
        if (children.at(i)->type() == 3) {//3 for QGraphicsRectItem
            delete children.at(i);

        }
    }
    VleLineEditItem* nameItem = getNameItem();
    new QGraphicsRectItem(
            QRectF(0,0,nameItem->boundingRect().width(),
                    nameItem->boundingRect().height()), this);
    QGraphicsRectItem* compRect = getRectangle();
    compRect->setPen(penContourObj(isSelected()));
    getNameItem()->setDefaultTextColor(colorObj(isSelected()));
}

QString
DecisionActivity::name() const
{
    return DomFunctions::attributeValue(node, "name");
}

QGraphicsRectItem*
DecisionActivity::getRectangle() const
{
    QList<QGraphicsItem *>  children = childItems();
    for (int i =0; i<children.length(); i++) {
        if (children.at(i)->type() == 3) {//3 for QGraphicsRectItem
            return static_cast<QGraphicsRectItem*>(children.at(i));
        }
    }
    return 0;
}




VleLineEditItem*
DecisionActivity::getNameItem() const
{
    QList<QGraphicsItem *>  children = childItems();
    for (int i =0; i<children.length(); i++) {
        if (children.at(i)->type() == 8) {//8 for VleLineEditItem
            return static_cast<VleLineEditItem*>(children.at(i));
        }
    }
    return 0;
}

void
DecisionActivity::setNameEdition(bool val)
{
    VleLineEditItem* textItem = getNameItem();
    if (val and textItem->textInteractionFlags() == Qt::NoTextInteraction) {
        textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
        textItem->setFocus(Qt::ActiveWindowFocusReason); // this gives the item keyboard focus
        textItem->setSelected(true); // this ensures that itemChange() gets called when we click out of the item
    } else {
        QTextCursor cursor(textItem->textCursor());
        cursor.clearSelection();
        textItem->setTextCursor(cursor);
        textItem->setTextInteractionFlags(Qt::NoTextInteraction);
        textItem->setSelected(false);
        textItem->clearFocus();
        if (name() != getNameItem()->toPlainText()) {
            dm->renameActivityToDoc(name(), getNameItem()->toPlainText());
            update();
        }

    }
}

DecisionPrecedence::DecisionPrecedence(DecisionActivity* _first,
         DecisionActivity* _second): QGraphicsLineItem(), first(_first),
                 second(_second)
{
    QGraphicsItem::setFlag(QGraphicsItem::ItemIsSelectable, true);
}

DecisionPrecedence::~DecisionPrecedence()
{
}

int
DecisionPrecedence::type() const
{
    return PrecedencePath;
}

QRectF
DecisionPrecedence::boundingRect() const
{
    qreal extra = (pen().width() + 20) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
            line().p2().y() - line().p1().y()))
            .normalized()
            .adjusted(-extra, -extra, extra, extra);
}

QPainterPath
DecisionPrecedence::shape() const
{
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon(arrowHead);
    return path;
}

void
DecisionPrecedence::paint(QPainter* painter,
        const QStyleOptionGraphicsItem */*option*/,
        QWidget */*widget*/)
{
    if ( first && second ) { // could not understand why but this is necessary

    qreal arrowSize = 10;
    painter->setPen(penPrecedencePath(isSelected()));
    setLine(QLineF(intersection(second, centerQGraphicsItem(first)),
                   intersection(first, centerQGraphicsItem(second))));

    double angle = ::acos(line().dx() / line().length());
    double Pi = std::acos(-1);

    if (line().dy() >= 0) {
        angle = (Pi * 2) - angle;
    }

    QPointF arrowP1 = line().p1() + QPointF(std::sin(angle + Pi / 3) * arrowSize,
                                    cos(angle + Pi / 3) * arrowSize);
    QPointF arrowP2 = line().p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
                                    cos(angle + Pi - Pi / 3) * arrowSize);

    arrowHead.clear();
    arrowHead << line().p1() << arrowP1 << arrowP2;

    // Fill polygon
    QPainterPath path;
    path.addPolygon(arrowHead);

    // Draw polygon
    painter->drawPolygon(arrowHead);
    painter->fillPath(path, colorObj(isSelected()));
    painter->drawLine(line());
    }

}

void
DecisionPrecedence::update(const QRectF & /*rect*/)
{
}

QString
DecisionPrecedence::firstName() const
{
    return first->name();
}

QString
DecisionPrecedence::secondName() const
{
    return second->name();
}

/***************************************************/

DecisionScene::DecisionScene(DecisionPanel* p): decision(p)
{
}

void
DecisionScene::reload()
{
     clear();

     QDomNodeList actsXml = decision->dataMetadata->activitiesFromDoc();
     for (int i = 0; i < actsXml.length(); i++) {

         QDomNode act = actsXml.item(i);
         QString name = act.attributes().namedItem("name").nodeValue();
         DecisionActivity* actItem = new DecisionActivity(act,
                                                          decision->dataMetadata);
         addItem(actItem);
         QPointF pos(act.attributes().namedItem("x").nodeValue().toFloat(),
                     act.attributes().namedItem("y").nodeValue().toFloat());
         actItem->setPos(pos);

     }

     reloadPrecedences();
}

void
DecisionScene::reloadPrecedences()
{
    clearPrecedences();
    QDomNodeList precedencesXml = decision->dataMetadata->PrecedencesFromDoc();
    for (int i = 0; i < precedencesXml.length(); i++) {
        QDomNode precedence = precedencesXml.item(i);
        QString firstName = precedence.attributes().namedItem("first").nodeValue();
        DecisionActivity* firstActItem = itemActivity(firstName);
        QString secondName = precedence.attributes().namedItem("second").nodeValue();
        DecisionActivity* secondActItem = itemActivity(secondName);

        addItem(new DecisionPrecedence(firstActItem, secondActItem));
    }
}

void
DecisionScene::unselectAllItems()
{
    QList<QGraphicsItem *> itemsIn = items();
     for (auto it : itemsIn) {
         if (it->type() == Activity){
             DecisionActivity* act =
                     static_cast<DecisionActivity*>(it);
             act->setSelected(false);
             act->update();
         } else if (it->type() == PrecedencePath){
             DecisionPrecedence* precedence =
                     static_cast<DecisionPrecedence*>(it);
             precedence->setSelected(false);
             precedence->update();
         }
     }
}

void
DecisionScene::removeNameEditionMode()
{

     QList<QGraphicsItem*>  children = items();
     for (int i =0; i<children.length(); i++) {
         if (children.at(i)) {
             if (children.at(i)->type() == Activity) {
                 static_cast<DecisionActivity*>(
                     children.at(i))->setNameEdition(false);
             }
         }
     }
}

void
DecisionScene::clearPrecedences()
{
    QList<QGraphicsItem *> itemsIn = items();

    for (auto it : itemsIn) {
        if (it->type() == PrecedencePath) {
            removeItem(it);
        }
    }
}

DecisionActivity*
DecisionScene::itemActivity(QString actName)
{
    QList<QGraphicsItem *> itemsIn = items();
    for (auto it : itemsIn) {
        if (it->type() == Activity){
            DecisionActivity* act =
                    static_cast<DecisionActivity*>(it);
            if (act->name() == actName) {
                return act;
            }
        }
    }
    return 0;
}

void
DecisionScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    QList<QGraphicsItem*> sels = items(event->scenePos());
    DecisionActivity* selAct = 0;
    DecisionPrecedence* selPrecedence = 0;
    for (auto it : sels) {
        if (it->type() == Activity) {
            selAct = static_cast<DecisionActivity*>(it);
            selPrecedence = 0;
        } else if (it->type() == PrecedencePath) {
             selAct = 0;
             selPrecedence = static_cast<DecisionPrecedence*>(it);
        }
    }

    QSet<QString> curr_acts = decision->dataMetadata->activityNames();

    QMenu menu;
    QAction* action;
    QMenu* submenu;
    QMap<QString, QVariant> action_map;

    QString obj_name = selAct ? selAct->name() : "";
    QString obj_first = selPrecedence ? selPrecedence->firstName() : "";
    QString obj_second = selPrecedence ? selPrecedence->secondName() : "";

    action = menu.addAction("Edit name");
    action_map.insert("code", QVariant(0));
    action_map.insert("name", obj_name);
    action->setData(action_map);
    action->setEnabled(action_map["name"].toString() != "");
    action = menu.addAction("Remove");
    action_map.clear();
    action_map.insert("code", QVariant(1));
    action_map.insert("name", obj_name);
    action->setData(action_map);
    menu.addSeparator();
    action = menu.addAction("Add Activity");
    action_map.clear();
    action_map.insert("code", QVariant(2));
    action->setData(action_map);
    menu.addSeparator();
    submenu = menu.addMenu("Add constraint to");
    submenu->setEnabled(selAct != 0 and curr_acts.size() > 0);
    if (submenu->isEnabled()) {
        curr_acts.remove(selAct->name());
        for (auto act : curr_acts) {
            action = submenu->addAction(act);
            action->setCheckable(true);
            action->setChecked(decision->dataMetadata->existPrecedenceToDoc(selAct->name(), act));
            action_map.clear();
            action_map.insert("code", QVariant(5));
            action_map.insert("name", QVariant(act));
            action->setData(action_map);
        }
    }
    submenu = menu.addMenu("Add constraint from");
    submenu->setEnabled(selAct != 0 and curr_acts.size() > 0);
    if (submenu->isEnabled()) {
        for (auto act : curr_acts) {
            action = submenu->addAction(act);
            action->setCheckable(true);
            action->setChecked(decision->dataMetadata->existPrecedenceToDoc(act, selAct->name()));
            action_map.clear();
            action_map.insert("code", QVariant(6));
            action_map.insert("name", QVariant(act));
            action->setData(action_map);
        }
    }

    action = menu.exec(event->screenPos());
    if (action) {
        int actCode = action->data().toMap().find("code")->toInt();
        switch (actCode) {
        case 0:{
            if (selAct) {
                selAct->setNameEdition(true);
            }
            break;
        }
        case 1:{
            if (selPrecedence) {
                decision->dataMetadata->rmPrecedenceToDoc(obj_first, obj_second);
            }
            if (selAct) {
                clearPrecedences();

                decision->dataMetadata->rmActivityToDoc(obj_name);
            }
            reload();
            emit decision->undoAvailable(true);
            break;
        }
        case 2:{
            decision->dataMetadata->addActivityToDoc(
                decision->dataMetadata->newActivityNameToDoc(),
                    event->scenePos());
            reload();
            emit decision->undoAvailable(true);
            break;
        }
        case 5:{
            QString actName = action->data().toMap().find("name")->toString();
            if (action->isChecked()) {
                decision->dataMetadata->addPrecedenceToDoc(selAct->name(), actName);
            } else {
                decision->dataMetadata->rmPrecedenceToDoc(selAct->name(), actName);
            }
            reload();
            emit decision->undoAvailable(true);
            break;
        }
        case 6:{
            QString actName = action->data().toMap().find("name")->toString();
            if (action->isChecked()) {
                decision->dataMetadata->addPrecedenceToDoc(actName, selAct->name());
            } else {
                decision->dataMetadata->rmPrecedenceToDoc(actName, selAct->name());
            }
            reload();
            emit decision->undoAvailable(true);
            break;
        }
        default:
            break;
        }
    }
}

void
DecisionScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {
        removeNameEditionMode();

        QList<QGraphicsItem*> items = this->items(mouseEvent->scenePos());
        QList<QGraphicsItem*> itemsAll = this->items();

        if(items.length() == 0) {
            unselectAllItems();
            emit decision->left->nothingSelected();
            return ;
        }

        //Look for a drag
        for (int i =0; i< items.length(); i++) {
            QGraphicsItem* it = items.at(i);

            if (it->type() == Activity){//activity
                DecisionActivity* act =
                        static_cast<DecisionActivity*>(it);

                if (not act->isSelected()) {
                    unselectAllItems();
                    act->setSelected(true);
                    act->update();
                    emit decision->left->actSelected(act->name());
                }
                QDrag* drag = new QDrag(mouseEvent->widget());
                drag->setMimeData(new QMimeData);
                QObject::connect(drag, SIGNAL(destroyed(QObject *)),
                        this, SLOT  (onDragDestroyed(QObject *)));
                drag->start();
                return;
            } else if (it->type() == PrecedencePath) {
                 DecisionPrecedence* precedence =
                     static_cast<DecisionPrecedence*>(it);
                 if (not precedence->isSelected()) {
                     unselectAllItems();
                     precedence->setSelected(true);
                     precedence->update();
                     emit decision->left->precedenceSelected(precedence->firstName(),
                                                             precedence->secondName());
                 }
                 return;
            }
        }
    }
}

void
DecisionScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * /*mouseEvent*/)
{
}

void
DecisionScene::dragEnterEvent(QGraphicsSceneDragDropEvent * /*event*/)
{
}

void
DecisionScene::dragLeaveEvent(QGraphicsSceneDragDropEvent * /*event*/)
{
}

void
DecisionScene::dragMoveEvent(QGraphicsSceneDragDropEvent * event)
{
    QList<QGraphicsItem*> sel = selectedItems();
    if (sel.size() != 1) {
        return;
    }
    if (sel.at(0)->type() == Activity) {
        DecisionActivity* act =
                static_cast<DecisionActivity*>(sel.at(0));
        QPointF p = event->scenePos();
        decision->dataMetadata->setPositionToActivity(
                act->name(), p);
        act->setPos(p);
        reloadPrecedences();
    }
}



void
DecisionScene::dropEvent(QGraphicsSceneDragDropEvent * /*event*/)
{
}

void
DecisionScene::onDragDestroyed(QObject* /*obj*/)
{
}

/*---------------------------------------------------*/

DecisionLeftWidget::DecisionLeftWidget(DecisionPanel* d):
    QWidget(), ui(new Ui::leftWidget), mScene(new DecisionScene(d)),
    decision(d)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(mScene);

    QObject::connect(ui->content, SIGNAL(currentChanged(int)),
                     this, SLOT(onTabChanged(int)));

    QObject::connect(ui->tablePredicates,
                     SIGNAL(customContextMenuRequested(const QPoint&)),
                     this, SLOT(onTablePredicatesMenu(const QPoint&)));

    QObject::connect(ui->treeRules,
                     SIGNAL(customContextMenuRequested(const QPoint&)),
                     this, SLOT(onRulesMenu(const QPoint&)));

    QObject::connect(ui->treeRules,
                     SIGNAL(itemChanged(QTreeWidgetItem*, int)),
                     this,
                     SLOT(onItemChanged(QTreeWidgetItem*, int)));

    QObject::connect(ui->DTOutput,
                     SIGNAL(clicked()),
                     this,
                     SLOT(onOutputsType()));

    QObject::connect(ui->EOutput,
                     SIGNAL(clicked()),
                     this,
                     SLOT(onOutputsType()));
}

DecisionLeftWidget::~DecisionLeftWidget()
{
    delete mScene;
    delete ui;
}

void
DecisionLeftWidget::onOutputsType()
{
    QString outputType;
    if (ui->DTOutput->isChecked()) {
        outputType = "discrete-time";
    } else {
        outputType = "event";
    }
    if (outputType !=  decision->dataMetadata->getOutputsTypeToDoc()) {
        decision->dataMetadata->setOutputsTypeToDoc(outputType);
        emit decision->undoAvailable(true);
    }
}

void
DecisionLeftWidget::onItemChanged(QTreeWidgetItem* item, int /*column*/)
{
    QList<QVariant> qVList = item->data(0, Qt::UserRole).toList();
    QVariant vItemType = qVList.at(0);
    RuleTreeType curItemType = (RuleTreeType)vItemType.toInt();
    QVariant vItemVal = qVList.at(1);
    QString curItemValue = vItemVal.toString();
    switch (curItemType) {
    case ERule: {
        if (curItemValue != item->text(0)) {
            decision->dataMetadata->renameRuleToDoc(curItemValue, item->text(0));
        }
        break;
    }
    default:
        break;
    }
}

void
DecisionLeftWidget::onTablePredicatesMenu(const QPoint& pos)
{
    QPoint globalPos = ui->tablePredicates->viewport()->mapToGlobal(pos);
    QModelIndex index = ui->tablePredicates->indexAt(pos);

    QString predicateName = "";

    if (index.row() >= 0 ){
        VleLineEdit* item = (VleLineEdit*)ui->tablePredicates->cellWidget(
            index.row(), 0);
        predicateName = item->text();
    }

    QAction* action;
    QMenu menu;
    action = menu.addAction("Add");
    action->setData(1);
    action = menu.addAction("Remove");
    action->setData(2);
    QAction* selAction = menu.exec(globalPos);
    if (selAction) {
        int actCode =  selAction->data().toInt();
        switch(actCode) {
        case 1: //Add Predicate
            decision->dataMetadata->addPredicateToDoc(decision->dataMetadata->newPredicateNameToDoc());
            mCurrPredicate = predicateName;
            reload();
            emit decision->undoAvailable(true);
            break;
        case 2: //Remove
            decision->dataMetadata->rmPredicateToDoc(predicateName);
            mCurrPredicate = "";
            reload();
            emit decision->undoAvailable(true);
            break;
        }
    }
}

void
DecisionLeftWidget::reload()
{
    bool oldBlock1 = ui->DTOutput->blockSignals(true);
    bool oldBlock2 = ui->EOutput->blockSignals(true);
    if (decision->dataMetadata->getOutputsTypeToDoc() == "discrete-time") {
        ui->DTOutput->setChecked(true);
        ui->EOutput->setChecked(false);
    } else {
        ui->DTOutput->setChecked(false);
        ui->EOutput->setChecked(true);
    }
    ui->DTOutput->blockSignals(oldBlock1);
    ui->EOutput->blockSignals(oldBlock2);

    reloadRules();
    reloadPredicates();
    mScene->reload();
}

void
DecisionLeftWidget::reloadRules()
{
    ui->treeRules->clear();
    ui->treeRules->setColumnCount(1);

    QSet<QString> rules = decision->dataMetadata->rulesNames();
    QList<QTreeWidgetItem*> rulesItems;
    QSet<QString>::iterator itb = rules.begin();
    QSet<QString>::iterator ite = rules.end();
    for (; itb != ite; itb++) {
        QTreeWidgetItem* newRuleItem =
            newItem(ERule, *itb);
        reloadPredicates(*itb, newRuleItem);
        rulesItems.append(newRuleItem);
    }
    ui->treeRules->insertTopLevelItems(0, rulesItems);
    ui->treeRules->expandAll();
}

void
DecisionLeftWidget::reloadPredicates(const QString& ruleName,
                                     QTreeWidgetItem* ruleItem)
{
    if (ruleItem == 0) {
        QList<QTreeWidgetItem*> listWidgetItem =
            ui->treeRules->findItems(ruleName, Qt::MatchExactly, 0);
        if (listWidgetItem.count() <= 0) {
            return;
        }
        ruleItem = listWidgetItem.at(0);
    }

    while (ruleItem->childCount()) {
        QTreeWidgetItem* child = ruleItem->takeChild(0);
        delete child;
    }

    QSet<QString> predicates = decision->dataMetadata->predicatesNamesRule(ruleName);
    QSet<QString>::iterator itb = predicates.begin();
    QSet<QString>::iterator ite = predicates.end();
    for (; itb != ite; itb++) {
        QTreeWidgetItem* newPredicateItem =
            newItem(EPredicate, *itb);
        ruleItem->addChild(newPredicateItem);
    }
}

void
DecisionLeftWidget::reloadPredicates()
{
    ui->tablePredicates->clearContents();
    ui->tablePredicates->setRowCount(0);

    QDomNodeList predicatesXml = decision->dataMetadata->predicatesFromDoc();
    for (int i = 0; i < predicatesXml.length(); i++) {
        QDomNode predicate = predicatesXml.item(i);
        QString name = predicate.attributes().namedItem("name").nodeValue();
        ui->tablePredicates->insertRow(i);
        insertRowPred(i, name);
    }
}

void
DecisionLeftWidget::insertRowPred(int row, const QString& name)
{
    QString op = decision->dataMetadata->getPredicateOperator(name);
    QString leftType = decision->dataMetadata->getPredicateLeftType(name);
    QString rightType = decision->dataMetadata->getPredicateRightType(name);
    QString rightValue = decision->dataMetadata->getPredicateRightValue(name);
    QString leftValue = decision->dataMetadata->getPredicateLeftValue(name);

    int col = 0;

    //insert Name
    {
        col = 0;
        QString id = QString("%1,%2").arg(row).arg(col);
        VleLineEdit* w = new VleLineEdit(ui->tablePredicates, name, id, false);
        ui->tablePredicates->setCellWidget(row, col, w);
        ui->tablePredicates->setItem(row, col, new QTableWidgetItem);
        QObject::connect(w, SIGNAL(
                textUpdated(const QString&, const QString&, const QString&)),
                this, SLOT(
                onPredNameUpdated(const QString&, const QString&, const QString&)));
        QObject::connect(w, SIGNAL(selected(const QString&)),
                this, SLOT(onSelected(const QString&)));
    }
    // operator
    {
         col = 3;
         QString id = QString("%1,%2").arg(row).arg(col);
         VleCombo* w = new VleCombo(ui->tablePredicates, id);
         QList <QString> l;
         l.append("<");
         l.append(">");
         l.append("<=");
         l.append(">=");
         l.append("==");
         l.append("!=");
         w->addItems(l);
         w->setCurrentIndex(w->findText(op));
         ui->tablePredicates->setCellWidget(row, col, w);
         ui->tablePredicates->setItem(row, col, new QTableWidgetItem);

         QObject::connect(w, SIGNAL(valUpdated(const QString&, const QString&)),
                 this, SLOT(onOperatorUpdated(const QString&, const QString&)));
         QObject::connect(w, SIGNAL(selected(const QString&)),
                 this, SLOT(onSelected(const QString&)));
    }
    // LeftType
    {
         col = 1;
         QString id = QString("%1,%2").arg(row).arg(col);
         VleCombo* w = new VleCombo(ui->tablePredicates, id);
         QList <QString> l;
         l.append("Val");
         l.append("Var");
         w->addItems(l);
         w->setCurrentIndex(w->findText(leftType));
         ui->tablePredicates->setCellWidget(row, col, w);
         ui->tablePredicates->setItem(row, col, new QTableWidgetItem);

         QObject::connect(w, SIGNAL(valUpdated(const QString&, const QString&)),
                 this, SLOT(onLeftTypeUpdated(const QString&, const QString&)));
         QObject::connect(w, SIGNAL(selected(const QString&)),
                 this, SLOT(onSelected(const QString&)));
    }
    // rightType
    {
         col = 4;
         QString id = QString("%1,%2").arg(row).arg(col);
         VleCombo* w = new VleCombo(ui->tablePredicates, id);
         QList <QString> l;
         l.append("Val");
         l.append("Var");
         w->addItems(l);
         w->setCurrentIndex(w->findText(rightType));
         ui->tablePredicates->setCellWidget(row, col, w);
         ui->tablePredicates->setItem(row, col, new QTableWidgetItem);

         QObject::connect(w, SIGNAL(valUpdated(const QString&, const QString&)),
                 this, SLOT(onRightTypeUpdated(const QString&, const QString&)));
         QObject::connect(w, SIGNAL(selected(const QString&)),
                 this, SLOT(onSelected(const QString&)));
    }
    //insert leftValue
    {
        col = 2;
        QString id = QString("%1,%2").arg(row).arg(col);
        if (leftType == "Val") {
            VleDoubleEdit* w = new VleDoubleEdit(ui->tablePredicates, leftValue.toDouble(), id, true);
            ui->tablePredicates->setCellWidget(row, col, w);
            ui->tablePredicates->setItem(row, col, new QTableWidgetItem);
            QObject::connect(w, SIGNAL(
                                 valUpdated(const QString&, double)),
                             this, SLOT(
                                 onLeftNumUpdated(const QString&, double)));
            QObject::connect(w, SIGNAL(selected(const QString&)),
                             this, SLOT(onSelected(const QString&)));
        } else {
            VleLineEdit* w = new VleLineEdit(ui->tablePredicates, leftValue, id, false);
            ui->tablePredicates->setCellWidget(row, col, w);
            ui->tablePredicates->setItem(row, col, new QTableWidgetItem);
            QObject::connect(w, SIGNAL(
                                 textUpdated(const QString&, const QString&, const QString&)),
                             this, SLOT(
                                 onLeftNameUpdated(const QString&, const QString&, const QString&)));
            QObject::connect(w, SIGNAL(selected(const QString&)),
                             this, SLOT(onSelected(const QString&)));
        }
    }
    //insert rightValue
    {
        col = 5;
        QString id = QString("%1,%2").arg(row).arg(col);
        if (rightType == "Val") {
            VleDoubleEdit* w = new VleDoubleEdit(ui->tablePredicates, rightValue.toDouble(), id, true);
            ui->tablePredicates->setCellWidget(row, col, w);
            ui->tablePredicates->setItem(row, col, new QTableWidgetItem);
            QObject::connect(w, SIGNAL(
                                 valUpdated(const QString&, double)),
                             this, SLOT(
                                 onRightNumUpdated(const QString&, double)));
            QObject::connect(w, SIGNAL(selected(const QString&)),
                             this, SLOT(onSelected(const QString&)));
        } else {
            VleLineEdit* w = new VleLineEdit(ui->tablePredicates, leftValue, id, false);
            ui->tablePredicates->setCellWidget(row, col, w);
            ui->tablePredicates->setItem(row, col, new QTableWidgetItem);
            QObject::connect(w, SIGNAL(
                                 textUpdated(const QString&, const QString&, const QString&)),
                             this, SLOT(
                                 onRightNameUpdated(const QString&, const QString&, const QString&)));
            QObject::connect(w, SIGNAL(selected(const QString&)),
                             this, SLOT(onSelected(const QString&)));
        }
    }
}

void
DecisionLeftWidget::onOperatorUpdated(const QString& id, const QString& val)
{
    QStringList split = id.split(",");
    int row = split.at(0).toInt();
    mCurrPredicate =  getTextEdit(row, 0)->text();
    decision->dataMetadata->setPredicateOperator(mCurrPredicate, val);
}

void
DecisionLeftWidget::onLeftTypeUpdated(const QString& id, const QString& val)
{
    QStringList split = id.split(",");
    int row = split.at(0).toInt();
    mCurrPredicate =  getTextEdit(row, 0)->text();
    decision->dataMetadata->setPredicateLeftType(mCurrPredicate, val);
    QString previous = decision->dataMetadata->getPredicateLeftValue(mCurrPredicate);
    if (val == "Val") {
        decision->dataMetadata->setPredicateLeftValue(mCurrPredicate, val, previous, "0.");
    } else {
        decision->dataMetadata->setPredicateLeftValue(mCurrPredicate, val, previous, "variableName");
    }
}

void
DecisionLeftWidget::onRightTypeUpdated(const QString& id, const QString& val)
{
    QStringList split = id.split(",");
    int row = split.at(0).toInt();
    mCurrPredicate =  getTextEdit(row, 0)->text();
    decision->dataMetadata->setPredicateRightType(mCurrPredicate, val);
    QString previous = decision->dataMetadata->getPredicateRightValue(mCurrPredicate);
    if (val == "Val") {
        decision->dataMetadata->setPredicateRightValue(mCurrPredicate, val, previous, "0.");
    } else {
        decision->dataMetadata->setPredicateRightValue(mCurrPredicate, val, previous, "variableName");
    }

}

void
DecisionLeftWidget::onLeftNameUpdated(const QString& /*id*/,
                                      const QString& oldname,
                                      const QString& newname)
{
    if (oldname != newname) {
        decision->dataMetadata->setPredicateLeftValue(mCurrPredicate, "rename" ,oldname, newname);
    }
}

void
DecisionLeftWidget::onRightNameUpdated(const QString& /*id*/,
                                      const QString& oldname,
                                      const QString& newname)
{
    if (oldname != newname) {
        decision->dataMetadata->setPredicateRightValue(mCurrPredicate, "rename" ,oldname, newname);
    }
}

void
DecisionLeftWidget::onLeftNumUpdated(const QString& id,
                                          double val)
{
    QStringList split = id.split(",");
    int row = split.at(0).toInt();
    mCurrPredicate =  getTextEdit(row, 0)->text();
    decision->dataMetadata->setPredicateLeftValue(mCurrPredicate, "", "", QString::number(val));
}

void
DecisionLeftWidget::onRightNumUpdated(const QString& id,
                                          double val)
{
    QStringList split = id.split(",");
    int row = split.at(0).toInt();
    mCurrPredicate =  getTextEdit(row, 0)->text();
    decision->dataMetadata->setPredicateRightValue(mCurrPredicate, "", "", QString::number(val));
}

void
DecisionLeftWidget::onPredNameUpdated(const QString& /*id*/,
                                      const QString& oldname,
                                      const QString& newname)
{
    if (oldname != newname) {
        decision->dataMetadata->renamePredicateToDoc(oldname, newname);
    }
    mCurrPredicate =  "";
    reload();
}

void
DecisionLeftWidget::onSelected(const QString& id)
{
    QStringList split = id.split(",");
    int row = split.at(0).toInt();
    mCurrPredicate = getTextEdit(row, 0)->text();
}

VleLineEdit*
DecisionLeftWidget::getTextEdit(int row, int col)
{
    return qobject_cast<VleLineEdit*>(
        ui->tablePredicates->cellWidget(row,col));
}

int
DecisionLeftWidget::itemType(const QTreeWidgetItem* item) const
{
    QList<QVariant> qVList = item->data(0, Qt::UserRole).toList();
    QVariant vItemType = qVList.at(0);
    return (RuleTreeType)vItemType.toInt();
}

QTreeWidgetItem*
DecisionLeftWidget::newItem(const RuleTreeType type, const QString name)
{
    QTreeWidgetItem* newItem = new QTreeWidgetItem();

    newItem->setText(0, name);
    QList<QVariant> dataList;
    dataList.append(type);
    dataList.append(name);
    QVariant data(dataList);
    newItem->setData(0, Qt::UserRole, data);

    switch (type) {
    case ERule: {
        newItem->setIcon(0, *(new QIcon(":/icon/resources/bricks.png")));
        newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
        break;
    }
    case EPredicate: {
        newItem->setIcon(0, *(new QIcon(":/icon/resources/cog.png")));
        newItem->setFlags(newItem->flags());
        break;
    }
    case ENone: {
        break;
    }
    }
    return newItem;
}

void
DecisionLeftWidget::onRulesMenu(const QPoint pos)
{
    QPoint globalPos = ui->treeRules->mapToGlobal(pos);
    QModelIndex index = ui->treeRules->indexAt(pos);

    int rulesSelected = 0;
    int predicatesSelected = 0;

    foreach (QTreeWidgetItem* item, ui->treeRules->selectedItems()) {
        int t = itemType(item);
        if (t == ERule)
            rulesSelected++;
        if (t == EPredicate)
            predicatesSelected++;
    }

    QModelIndexList selection = ui->tablePredicates->selectionModel()->selectedRows();

    QList<QString> predicateList;

    for(int i=0; i< selection.count(); i++) {
        QModelIndex index = selection.at(i);
        VleLineEdit* item = (VleLineEdit*)ui->tablePredicates->cellWidget(
            index.row(), 0);

        predicateList.append(item->text());
    }

    QAction* lastAction;

    QMenu ctxMenu;
    lastAction = ctxMenu.addAction(tr("Add Rule"));
    lastAction->setData(1);
    lastAction->setEnabled(index.row() == -1);
    lastAction = ctxMenu.addAction(tr("Remove Rule"));
    lastAction->setData(2);
    lastAction->setEnabled(rulesSelected > 0 and index.row() != -1);
    lastAction = ctxMenu.addAction(tr("Add Predicates"));
    lastAction->setData(3);
    lastAction->setEnabled(rulesSelected > 0 and index.row() != -1);
    lastAction = ctxMenu.addAction(tr("Remove Predicates"));
    lastAction->setData(4);
    lastAction->setEnabled(predicatesSelected > 0 and index.row() != -1);

    QAction* selectedItem = ctxMenu.exec(globalPos);
    if (selectedItem) {
        int actCode = selectedItem->data().toInt();
        if (actCode == 1) {
            QString name = decision->dataMetadata->newRuleNameToDoc();
            decision->dataMetadata->addRuleToDoc(name);
            reloadRules();
        } else if (actCode == 2) {
            QList<QString> ruleList;
            foreach (QTreeWidgetItem* item, ui->treeRules->selectedItems()) {
                if (itemType(item) == ERule) {
                    ruleList.append(item->text(0));
                }
            }
            QString r;
            foreach (r, ruleList) {
                decision->dataMetadata->rmRuleToDoc(r);
            }
        } else if (actCode == 3) {
            QList<QString> ruleList;
            foreach (QTreeWidgetItem* item, ui->treeRules->selectedItems()) {
                if (itemType(item) == ERule) {
                    ruleList.append(item->text(0));
                }
            }
            QString r;
            foreach (r, ruleList) {
                QString p;
                foreach (p, predicateList) {
                    if (not decision->dataMetadata->existPredicateRuleToDoc(r, p)) {
                        decision->dataMetadata->addPredicateRuleToDoc(r, p);
                    }
                }
            }
        } else if (actCode == 4) {
            foreach (QTreeWidgetItem* item, ui->treeRules->selectedItems()) {
                if (itemType(item) == EPredicate) {
                    QTreeWidgetItem* rule = item->parent();
                    decision->dataMetadata->rmPredicateRuleToDoc(rule->text(0),
                                                                 item->text(0));
                }
            }
        }
    }
}

void
DecisionLeftWidget::onDmModified(int modifType)
{
    switch((MODIF_TYPE) modifType) {
    case PREDICATE:
        reloadPredicates();
        break;
    case RENAME:
    case RENAMEPREDICATE:
        reloadPredicates();
        reloadRules();
        break;
    case RENAMERULE:
    case RULES:
        reloadRules();
        break;
    case OUTPUTS:
    case MOVE_OBJ:
        break;
    default:
        reload();
        break;
    }
}

void
DecisionLeftWidget::onTabChanged(int)
{
    QString tab = ui->content->tabText(ui->content->currentIndex());
}

}} //namespaces
