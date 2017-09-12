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

#include <sstream>
#include <iostream>

#include <QFileInfo>
#include <QDir>
#include <QtXml/QDomNode>

#include <vle/utils/Template.hpp>
#include <vle/utils/DateTime.hpp>
#include <vle/gvle/vlevpz.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Set.hpp>

#include "vledmdd.h"

namespace vle {
namespace gvle {

using namespace vle::gvle;
namespace vv = vle::value;

vleDomDmDD::vleDomDmDD(QDomDocument* doc): DomObject(doc)
{

}

vleDomDmDD::~vleDomDmDD()
{

}

QString
vleDomDmDD::getXQuery(QDomNode node)
{
    QString name = node.nodeName();
    if ((name == "dataModel") or
        (name == "dataPlugin") or
        (name == "activities") or
        (name == "predicates") or
        (name == "rules") or
        (name == "dynamic") or
        (name == "observable") or
        (name == "condition") or
        (name == "in") or
        (name == "out")) {
        return getXQuery(node.parentNode())+"/"+name;
    }
    //element identified by attribute name
    if ((name == "activity") or
        (name == "predicate") or
        (name == "rule")) {
        return getXQuery(node.parentNode())+"/"+name+"[@name=\""
            +DomFunctions::attributeValue(node,"name")+"\"]";
    }
    if (node.nodeName() == "vle_project_metadata") {
        return "./vle_project_metadata";
    }
    return "";
}

QDomNode
vleDomDmDD::getNodeFromXQuery(const QString& query,
        QDomNode d)
{
    //handle last
    if (query.isEmpty()) {
        return d;
    }

    QStringList queryList = query.split("/");
    QString curr = "";
    QString rest = "";
    int j=0;
    if (queryList.at(0) == ".") {
        curr = queryList.at(1);
        j=2;
    } else {
        curr = queryList.at(0);
        j=1;
    }
    for (int i=j; i<queryList.size(); i++) {
        rest = rest + queryList.at(i);
        if (i < queryList.size()-1) {
            rest = rest + "/";
        }
    }

    //handle first
    if (d.isNull()) {
        QDomNode rootNode = mDoc->documentElement();
        if (curr != "vle_project_metadata" or queryList.at(0) != ".") {
            return QDomNode();
        }
        return(getNodeFromXQuery(rest,rootNode));
    }

    //handle recursion with uniq node
    if ((curr == "dataModel") or
        (curr == "datPlugin") or
        (curr == "activities") or
        (curr == "predicates") or
        (curr == "rules") or
        (curr == "precedences") or
        (curr == "dynamic") or
        (curr == "observable") or
        (curr == "condition") or
        (curr == "in") or
        (curr == "out")) {
        return getNodeFromXQuery(rest, DomFunctions::obtainChild(d, curr, mDoc));
    }

    //handle recursion with nodes identified by name
    std::vector<QString> nodeByNames;
    nodeByNames.push_back(QString("activity"));
    nodeByNames.push_back(QString("predicate"));
    nodeByNames.push_back(QString("rule"));
    QString selNodeByName ="";
    for (unsigned int i=0; i< nodeByNames.size(); i++) {
        if (curr.startsWith(nodeByNames[i])) {
            selNodeByName = nodeByNames[i];
        }
    }
    if (not selNodeByName.isEmpty()) {
        QStringList currSplit = curr.split("\"");
        QDomNode selMod = DomFunctions::childWhithNameAttr(d,
                selNodeByName, currSplit.at(1));
        return getNodeFromXQuery(rest,selMod);
    }
    return QDomNode();
}

/************************************************************************/

vleDmDD::vleDmDD(const QString& srcpath, const QString& ompath,
        QString pluginName):
    mDocDm(0), mFileNameSrc(srcpath), mFileNameDm(ompath), mVdoDm(0),
    undoStackDm(0), waitUndoRedoDm(false),
    oldValDm(), newValDm(), mpluginName(pluginName)
{
    QFile file(mFileNameDm);
    if (file.exists()) {
        mDocDm = new QDomDocument("vle_project_metadata");
        QXmlInputSource source(&file);
        QXmlSimpleReader reader;
        mDocDm->setContent(&source, &reader);
    } else {
        xCreateDom();
    }

    mVdoDm = new vleDomDmDD(mDocDm);
    undoStackDm = new DomDiffStack(mVdoDm);
    undoStackDm->init(*mDocDm);

    QObject::connect(undoStackDm,
                SIGNAL(undoRedoVdo(QDomNode, QDomNode)),
                this, SLOT(onUndoRedoDm(QDomNode, QDomNode)));
    QObject::connect(undoStackDm,
                SIGNAL(undoAvailable(bool)),
                this, SLOT(onUndoAvailable(bool)));
}

QString
vleDmDD::toQString(const QDomNode& node) const
{
    QString str;
    QTextStream stream(&str);
    node.save(stream, node.nodeType());
    return str;
}

void
vleDmDD::xCreateDom()
{
    if (not mDocDm) {
        mDocDm = new QDomDocument("vle_project_metadata");
        QDomProcessingInstruction pi;
        pi = mDocDm->createProcessingInstruction("xml",
                "version=\"1.0\" encoding=\"UTF-8\" ");
        mDocDm->appendChild(pi);

        QDomElement vpmRoot = mDocDm->createElement("vle_project_metadata");
        // Save VPZ file revision
        vpmRoot.setAttribute("version", "1.x");
        // Save the author name (if known)
        vpmRoot.setAttribute("author", "meto");
        QDomElement xCondDataMod = mDocDm->createElement("dataModel");
        QDomElement xCondDataPlug = mDocDm->createElement("dataPlugin");
        vpmRoot.appendChild(xCondDataPlug);
        vpmRoot.appendChild(xCondDataMod);


        QDomElement xInsideElem;
        QDomElement xElem;
        QDomElement xDefModel = mDocDm->createElement("definition");
        xElem = mDocDm->createElement("activities");
        xDefModel.appendChild(xElem);
        xElem = mDocDm->createElement("predicates");
        xDefModel.appendChild(xElem);
        xElem = mDocDm->createElement("rules");
        xDefModel.appendChild(xElem);
        xElem = mDocDm->createElement("precedences");
        xDefModel.appendChild(xElem);
        vpmRoot.appendChild(xDefModel);

        QDomElement xConfModel = mDocDm->createElement("configuration");
        xElem = mDocDm->createElement("dynamic");
        xConfModel.appendChild(xElem);
        xElem = mDocDm->createElement("observable");
        xConfModel.appendChild(xElem);
        xElem = mDocDm->createElement("condition");
        xInsideElem = mDocDm->createElement("port");
        xInsideElem.setAttribute("name", "dyn_allow");
        xElem.appendChild(xInsideElem);
        xInsideElem = mDocDm->createElement("port");
        xInsideElem.setAttribute("name", "dyn_denys");
        xElem.appendChild(xInsideElem);
        xInsideElem = mDocDm->createElement("port");
        xInsideElem.setAttribute("name", "autoAck");
        xElem.appendChild(xInsideElem);
        xConfModel.appendChild(xElem);
        xElem = mDocDm->createElement("in");
        xConfModel.appendChild(xElem);
        xElem = mDocDm->createElement("out");
        xConfModel.appendChild(xElem);

        vpmRoot.appendChild(xConfModel);

        mDocDm->appendChild(vpmRoot);

        {
            QDomNode port = nodeCondPort("dyn_allow");
            QDomElement vale = mDocDm->createElement("boolean");
            vv::Boolean* value = new vv::Boolean(true);
            vleDomStatic::fillWithValue(*mDocDm, vale, *value);
            port.appendChild(vale);
        }
        {
            QDomNode port = nodeCondPort("autoAck");
            QDomElement vale = mDocDm->createElement("boolean");
            vv::Boolean* value = new vv::Boolean(true);
            vleDomStatic::fillWithValue(*mDocDm, vale, *value);
            port.appendChild(vale);
        }
        {
            QDomNode port = nodeCondPort("dyn_denys");
            QDomElement vale = mDocDm->createElement("set");
            port.appendChild(vale);
        }

        vleDomStatic::addObservablePort(*mDocDm, nodeObs(), "Knowledgebase");
        vleDomStatic::addObservablePort(*mDocDm, nodeObs(), "Activities");
    }
}

void
vleDmDD::xReadDom()
{
    if (mDocDm) {
        QFile file(mFileNameDm);
        mDocDm->setContent(&file);
    }
}

void
vleDmDD::addOutParActToDoc(const QString& actName, const QString& outParName)
{
    if (existOutParActToDoc(outParName)) {
        return;
    }

    QDomNode outParsNode = nodeOutParsAct(actName);

    undoStackDm->snapshot(outParsNode);

    //add Param
    QDomElement el = mDocDm->createElement("outputParam");
    el.setAttribute("name", outParName);
    el.setAttribute("value", QString::number(0.));
    outParsNode.appendChild(el);

    //add Port
    vleDomStatic::addPortToOutNode(*mDocDm, nodeOut(), outParName, 0);

    //add deny

    QDomNode condNode = nodeCond();
    vle::value::Map tofill;
    tofill.add("dyn_denys", vle::value::Set::create());
    QDomNode port = nodeCondPort("dyn_denys");
    QDomNode init_value = port.firstChildElement();
    std::unique_ptr<vle::value::Value> ddValue;
    ddValue = vleDomStatic::buildValue(init_value, false);
    const vle::value::Set& denys = ddValue->toSet();
    vle::value::Set::const_iterator idb = denys.begin();
    vle::value::Set::const_iterator ide = denys.end();
    for (; idb != ide; idb++) {
        tofill["dyn_denys"]->toSet().add(vle::value::String::create((*idb)->toString().value()));
    }
    tofill["dyn_denys"]->toSet().add(vle::value::String::create(outParName.toStdString()));
    vleDomStatic::fillConditionWithMap(*mDocDm, condNode, tofill);

    emit modified(OUTPUTS);
}

void
vleDmDD::addRuleActToDoc(const QString& actName, const QString& ruleName)
{
    if (existRuleActToDoc(actName, ruleName)) {
        return;
    }

    QDomNode rulesActNode = nodeRulesAct(actName);

    undoStackDm->snapshot(rulesActNode);

    //add Param
    QDomElement el = mDocDm->createElement("rule");
    el.setAttribute("name", ruleName);
    rulesActNode.appendChild(el);
    emit modified(OTHER);
}

void
vleDmDD::setValueOutParAct(const QString& actName, const QString& outParName,
                           double value, bool snap)
{
    if (not existOutParActToDoc(outParName)) {
        return;
    }

    QDomNode par = nodeOutParAct(actName, outParName);

    if (snap) {
        undoStackDm->snapshot(par);
    }

    DomFunctions::setAttributeValue(par, "value", QString::number(value));

    if (snap) {
        emit modified(OUTPUTS);
    }
}

void
vleDmDD::renameOutParActToDoc(const QString& actName,
                              const QString& oldOutParName,
                              const QString& newOutParName)
{
    QDomNode rootNode = mDocDm->documentElement();

    if (not existOutParActToDoc(oldOutParName)) {
        return ;

    }
    if (not existOutParActToDoc(actName, oldOutParName)) {
        return ;

    }

    QDomNode par = nodeOutParAct(actName, oldOutParName);

    undoStackDm->snapshot(rootNode);

    //manage deny

    QDomNode condNode = nodeCond();
    vle::value::Map tofill;
    tofill.add("dyn_denys", vle::value::Set::create());
    QDomNode port = nodeCondPort("dyn_denys");
    QDomNode init_value = port.firstChildElement();
    std::unique_ptr<vle::value::Value> ddValue;
    ddValue = vleDomStatic::buildValue(init_value, false);
    const vle::value::Set& denys = ddValue->toSet();
    vle::value::Set::const_iterator idb = denys.begin();
    vle::value::Set::const_iterator ide = denys.end();
    for (; idb != ide; idb++) {
        if (singleOutUsage(oldOutParName) and not outUsed(newOutParName) and
            (*idb)->toString().value() == oldOutParName.toStdString()) {
            tofill["dyn_denys"]->toSet().add(vle::value::String::create(newOutParName.toStdString()));
        } else if (not singleOutUsage(oldOutParName) and
                   (*idb)->toString().value() == oldOutParName.toStdString()) {
            tofill["dyn_denys"]->toSet().add(vle::value::String::create((*idb)->toString().value()));
        } else if ((*idb)->toString().value() != oldOutParName.toStdString()) {
            tofill["dyn_denys"]->toSet().add(vle::value::String::create((*idb)->toString().value()));
        }
    }
    if (not singleOutUsage(oldOutParName) and not outUsed(newOutParName)) {
        tofill["dyn_denys"]->toSet().add(vle::value::String::create(newOutParName.toStdString()));
    }

    vleDomStatic::fillConditionWithMap(*mDocDm, condNode, tofill);

    // manage port

    if (not singleOutUsage(oldOutParName) and not outUsed(newOutParName)) {
        vleDomStatic::addPortToOutNode(*mDocDm, nodeOut(), newOutParName, 0);
    } else if (not outUsed(newOutParName)) {
        vleDomStatic::renamePortToOutNode(nodeOut(), oldOutParName,
                                          newOutParName, 0);
    } else {
        vleDomStatic::rmPortToOutNode(nodeOut(),
                                      oldOutParName);
    }


    // manage activity
    DomFunctions::setAttributeValue(par, "name", newOutParName);

    emit modified(OUTPUTS);
}

QString
vleDmDD::newOutParToDoc()
{
    QDomNodeList outParList = nodeActs().toElement().elementsByTagName("outputParam");
    QString new_name = "outputParam";
    unsigned int id_name = 0;
    bool new_name_found = false;
    while (not new_name_found) {
        bool new_name_found_i = false;
        for (int j = 0; j < outParList.length(); j++) {
            QDomNode oPA = outParList.item(j);
            new_name_found_i =
                new_name_found_i or
                oPA.attributes().namedItem("name").nodeValue() == new_name;
        }

        if (new_name_found_i) {
            id_name++;
            new_name = "outputParam";
            new_name += "_";
            new_name += QVariant(id_name).toString();
        } else {
            new_name_found = true;
        }
    }
    return new_name;
}

bool
vleDmDD::existOutParActToDoc(QString actName,
                             QString outParName)
{
    QDomNodeList childs = nodeAct(actName).toElement().elementsByTagName("outputParam");
    for (int i = 0; i < childs.length(); i++) {
        QDomNode child = childs.item(i);
        if ((child.nodeName() == "outputParam") and
            (child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == outParName)) {
            return true;
        }
    }
    return false;
}

bool
vleDmDD::existOutParActToDoc(QString outParName)
{
    QDomNodeList childs = nodeActs().toElement().elementsByTagName("outputParam");
    for (int i = 0; i < childs.length(); i++) {
        QDomNode child = childs.item(i);
        if ((child.nodeName() == "outputParam") and
            (child.attributes().contains("name")) and
            (child.attributes().namedItem("name").nodeValue() == outParName)) {
            return true;
        }
    }
    return false;
}

bool
vleDmDD::existRuleActToDoc(QString actName, QString ruleName)
{
    QDomNode act = nodeAct(actName);

    if ( not act.isNull() ) {
        QDomNode rules = nodeRulesAct(actName);
        QDomNodeList childs = rules.childNodes();
        for (int i = 0; i < childs.length(); i++) {
            QDomNode child = childs.item(i);
            if ((child.nodeName() == "rule") and
                (child.attributes().contains("name")) and
                (child.attributes().namedItem("name").nodeValue() == ruleName)) {
                return true;
            }
        }
        return false;
    }
    return false;
}

void
vleDmDD::rmOutputParActToDoc(const QString& actName, const QString& outParName)
{
    if (not existOutParActToDoc(outParName)) {
        return;
    }

    undoStackDm->snapshot(mDocDm->documentElement());

    QDomNode parsNode = nodeOutParsAct(actName);
    QDomNode parNode = nodeOutParAct(actName, outParName);
    parsNode.removeChild(parNode);

    vleDomStatic::rmPortToOutNode(nodeOut(), outParName, 0);

    //manage deny

    QDomNode condNode = nodeCond();
    vle::value::Map tofill;
    tofill.add("dyn_denys", vle::value::Set::create());
    QDomNode port = nodeCondPort("dyn_denys");
    QDomNode init_value = port.firstChildElement();
    std::unique_ptr<vle::value::Value> ddValue;
    ddValue = vleDomStatic::buildValue(init_value, false);
    const vle::value::Set& denys = ddValue->toSet();
    vle::value::Set::const_iterator idb = denys.begin();
    vle::value::Set::const_iterator ide = denys.end();
    for (; idb != ide; idb++) {
        if ((*idb)->toString().value() != outParName.toStdString()) {
            tofill["dyn_denys"]->toSet().add(vle::value::String::create((*idb)->toString().value()));
        }
    }
    vleDomStatic::fillConditionWithMap(*mDocDm, condNode, tofill);

    emit modified(OUTPUTS);
}

void
vleDmDD::rmRuleActToDoc(const QString& actName, const QString& ruleName)
{
    if (not existRuleActToDoc(actName, ruleName)) {
        return;
    }

    undoStackDm->snapshot(mDocDm->documentElement());

    QDomNode rulesNode = nodeRulesAct(actName);
    QDomNode ruleNode = nodeRuleAct(actName, ruleName);
    rulesNode.removeChild(ruleNode);

    emit modified(OTHER);
}

QDomNodeList
vleDmDD::outParActFromDoc(const QString& actName)
{
    return nodeOutParsAct(actName).toElement().elementsByTagName("outputParam");
}

QDomNodeList
vleDmDD::ruleActFromDoc(const QString& actName)
{
    return nodeRulesAct(actName).toElement().elementsByTagName("rule");
}

QDomNode
vleDmDD::nodeOutParsAct(const QString& actName) const
{
    QDomNode act = nodeAct(actName);
    QDomNodeList childs = act.childNodes();
    for (int i = 0; i < childs.length(); i++) {
        QDomNode child = childs.item(i);
        if (child.nodeName() == "outputParams") {
            return child;
        }
    }
    qDebug() << ("Internal error in nodeOutParsAct (outputParams not found)");
    return QDomNode();
}

QDomNode
vleDmDD::nodeRulesAct(const QString& actName) const
{
    QDomNode act = nodeAct(actName);
    QDomNodeList childs = act.childNodes();
    for (int i = 0; i < childs.length(); i++) {
        QDomNode child = childs.item(i);
        if (child.nodeName() == "rulesAssigment") {
            return child;
        }
    }
    qDebug() << ("Internal error in nodeRulesAct (rules not found)");
    return QDomNode();
}

QDomNode
vleDmDD::nodeOutParAct(const QString& actName, const QString& outParName) const
{
    QDomNode pars = nodeOutParsAct(actName);
    return DomFunctions::childWhithNameAttr(pars , "outputParam", outParName);
}

QDomNode
vleDmDD::nodeRuleAct(const QString& actName, const QString& ruleName) const
{
    QDomNode rules = nodeRulesAct(actName);
    return DomFunctions::childWhithNameAttr(rules , "rule", ruleName);
}

QDomNodeList
vleDmDD::PrecedencesFromDoc()
{
    return nodePrecedences().toElement().elementsByTagName("precedence");
}

QString
vleDmDD::getPrecedenceType(const QString& firstName,
                           const QString& secondName)
{
    if (not existPrecedenceToDoc(firstName, secondName)) {
        return "";
    } else {
        QDomNode precedenceNode = nodePrecedence(firstName, secondName);
        return precedenceNode.attributes().namedItem("type").nodeValue();
    }
}

void vleDmDD::setPrecedenceType(const QString& firstName,
                                const QString& secondName,
                                int idType)
{
    if (not existPrecedenceToDoc(firstName, secondName)) {
        return;
    }

    QDomNode precedenceNode = nodePrecedence(firstName, secondName);

    undoStackDm->snapshot(precedenceNode);

    QString typeString;
    if (idType == 0) {
        typeString = "FS";
    } else if (idType == 1) {
        typeString = "SS";
    } else {
        typeString = "FF";
    }

    DomFunctions::setAttributeValue(precedenceNode, "type", typeString);
}

QString
vleDmDD::getPrecedenceMinLag(const QString& firstName,
                             const QString& secondName)
{
    if (not existPrecedenceToDoc(firstName, secondName)) {
        return "";
    } else {
        QDomNode precedenceNode = nodePrecedence(firstName, secondName);
        return precedenceNode.attributes().namedItem("mintimelag").nodeValue();
    }
}

void vleDmDD::setPrecedenceMinLag(const QString& firstName,
                                  const QString& secondName,
                                  int lag)
{
    if (not existPrecedenceToDoc(firstName, secondName)) {
        return;
    }

    QDomNode precedenceNode = nodePrecedence(firstName, secondName);

    undoStackDm->snapshot(precedenceNode);

    QString lagString = "";
    if (lag != 0) {
        lagString = QString::number(lag);
    }

    DomFunctions::setAttributeValue(precedenceNode, "mintimelag", lagString);
}

QString
vleDmDD::getPrecedenceMaxLag(const QString& firstName,
                             const QString& secondName)
{
    if (not existPrecedenceToDoc(firstName, secondName)) {
        return "";
    } else {
        QDomNode precedenceNode = nodePrecedence(firstName, secondName);
        return precedenceNode.attributes().namedItem("maxtimelag").nodeValue();
    }
}

void vleDmDD::setPrecedenceMaxLag(const QString& firstName,
                                  const QString& secondName,
                                  int lag)
{
    if (not existPrecedenceToDoc(firstName, secondName)) {
        return;
    }

    QDomNode precedenceNode = nodePrecedence(firstName, secondName);

    undoStackDm->snapshot(precedenceNode);

    QString lagString = "";
    if (lag != 0) {
        lagString = QString::number(lag);
    }

    DomFunctions::setAttributeValue(precedenceNode, "maxtimelag", lagString);
}

void
vleDmDD::addPrecedenceToDoc(const QString& firstName,
                            const QString& secondName)
{
    if (existPrecedenceToDoc(firstName, secondName)) {
        return;
    }

    QDomNode rootNode = mDocDm->documentElement();
    QDomNode precedencesNode = mDocDm->elementsByTagName("precedences").item(0);

    undoStackDm->snapshot(rootNode);

    QDomElement xElem;
    //add precedence
    QDomElement el = mDocDm->createElement("precedence");
    el.setAttribute("first", firstName);
    el.setAttribute("second", secondName);
    el.setAttribute("type", "FS");
    el.setAttribute("mintimelag", "");
    el.setAttribute("maxtimelag", "");
    precedencesNode.appendChild(el);

    emit modified(OTHER);
}

bool
vleDmDD::existPrecedenceToDoc(const QString& firstName,
			      const QString& secondName)
{
    QDomNode precedence = nodePrecedence(firstName, secondName);
    return not precedence.isNull();
}

void
vleDmDD::rmPrecedenceToDoc(const QString& firstName,
                           const QString& secondName)
{
    if (not existPrecedenceToDoc(firstName, secondName)) {
        return;
    }

    undoStackDm->snapshot(mDocDm->documentElement());

    QDomNode precedencesNode = nodePrecedences();
    QDomNode precedenceNode = nodePrecedence(firstName, secondName);
    precedencesNode.removeChild(precedenceNode);

    //emit modified(RENAME);
}

QDomNode
vleDmDD::nodePrecedences() const
{
    return mDocDm->elementsByTagName("precedences").item(0);
}

QDomNode
vleDmDD::nodePrecedence(const QString& firstName,
                        const QString& secondName) const
{
    QDomNode precedences = nodePrecedences();
    QDomNodeList childs = precedences.childNodes();
    for (int i = 0; i < childs.length(); i++) {
        QDomNode child = childs.item(i);
        if (child.nodeName() == "precedence" and
            child.attributes().contains("first") and
            child.attributes().namedItem("first").nodeValue() == firstName and
            child.attributes().contains("second") and
            child.attributes().namedItem("second").nodeValue() == secondName) {
            return child;
        }
    }
    return QDomNode();
}

void
vleDmDD::rmPrecedencesToDoc(const QString& activityName)
{

    undoStackDm->snapshot(mDocDm->documentElement());

    QDomNode precedences = nodePrecedences();
    QDomNodeList childs = precedences.childNodes();
    for (int i = 0; i < childs.length(); i++) {
        QDomNode child = childs.item(i);
        if (child.nodeName() == "precedence" and
            ((child.attributes().contains("first") and
             child.attributes().namedItem("first").nodeValue() == activityName) or
            (child.attributes().contains("second") and
             child.attributes().namedItem("second").nodeValue() == activityName))) {
            precedences.removeChild(child);
        }
    }

    emit modified(RENAME);
}

QDomNodeList
vleDmDD::RulesFromDoc()
{
    return nodeRules().toElement().elementsByTagName("rule");
}

void
vleDmDD::addRuleToDoc(const QString& ruleName)
{
    if (existRuleToDoc(ruleName)) {
        return;
    }

    QDomNode rootNode = mDocDm->documentElement();
    QDomNode rulesNode = mDocDm->elementsByTagName("rules").item(0);

    undoStackDm->snapshot(rootNode);

    QDomElement xElem;
    //add rule
    QDomElement el = mDocDm->createElement("rule");
    el.setAttribute("name", ruleName);
    rulesNode.appendChild(el);

    emit modified(OTHER);
}

void
vleDmDD::addPredicateRuleToDoc(const QString& ruleName,
                               const QString& predicateName)
{
    if (existPredicateRuleToDoc(ruleName, predicateName)) {
        return;
    }

    QDomNode ruleNode = nodeRule(ruleName);

    undoStackDm->snapshot(ruleNode);

    QDomElement xElem;
    //add rule
    QDomElement el = mDocDm->createElement("predicate");
    el.setAttribute("name", predicateName);
    ruleNode.appendChild(el);

    emit modified(OTHER);
}

void
vleDmDD::rmPredicateRuleToDoc(const QString& ruleName,
                              const QString& predicateName)
{
    if (not existPredicateRuleToDoc(ruleName, predicateName)) {
        return;
    }

    QDomNode ruleNode = nodeRule(ruleName);

    undoStackDm->snapshot(ruleNode);

    QDomNode predicateNode = nodePredicateRule(ruleName, predicateName);
    ruleNode.removeChild(predicateNode);

    emit modified(RULES);
}

bool
vleDmDD::existPredicateRuleToDoc(QString ruleName, QString predicateName)
{
    QDomNode rule = nodeRule(ruleName);

    if ( not rule.isNull() ) {
        QDomNodeList childs = rule.childNodes();
        for (int i = 0; i < childs.length(); i++) {
            QDomNode child = childs.item(i);
            if ((child.nodeName() == "predicate") and
                (child.attributes().contains("name")) and
                (child.attributes().namedItem("name").nodeValue() == predicateName)) {
                return true;
            }
        }
        return false;
    }
    return false;
}

QDomNode
vleDmDD::nodePredicateRule(const QString& ruleName,
                           const QString& predicateName) const
{
    QDomNode rule = nodeRule(ruleName);
    QDomNodeList childs = rule.childNodes();
    for (int i = 0; i < childs.length(); i++) {
        QDomNode child = childs.item(i);
        if (child.nodeName() == "predicate" and
            child.attributes().contains("name") and
            child.attributes().namedItem("name").nodeValue() == predicateName) {
            return child;
        }
    }
    return QDomNode();
}

QString vleDmDD::newRuleNameToDoc()
{
    return DomFunctions::childNameProvider(nodeRules(),"rule",
                                           "rule");
}

void vleDmDD::rmRuleToDoc(const QString& ruleName)
{
    if (not existRuleToDoc(ruleName)) {
        return;
    }

    undoStackDm->snapshot(mDocDm->documentElement());

    QDomNode rulesNode = nodeRules();
    QDomNode ruleNode = nodeRule(ruleName);
    rulesNode.removeChild(ruleNode);

    emit modified(RENAME);
}

void vleDmDD::renameRuleToDoc(const QString& oldName,
                              const QString& newName)
{
    QDomNode rootNode = mDocDm->documentElement();

    QDomNode rule = nodeRule(oldName);
    if (rule.isNull()) {
        return ;
    }

    if (existRuleToDoc(newName)) {
        emit modified(RENAMERULE);
        return;
    }

    undoStackDm->snapshot(rootNode);

    DomFunctions::setAttributeValue(rule, "name", newName);

    QSet<QString> activities = activityNames();
    QSet<QString>::iterator itb = activities.begin();
    QSet<QString>::iterator ite = activities.end();
    for (; itb != ite; itb++) {
        QDomNode ruleActNode = nodeRuleAct(*itb, oldName);
        DomFunctions::setAttributeValue(ruleActNode, "name", newName);
    }

    emit modified(RENAMERULE);
}

bool
vleDmDD::existRuleToDoc(const QString& ruleName)
{
    QDomNode rule = nodeRule(ruleName);
    return not rule.isNull();
}

QDomNode
vleDmDD::nodeRules() const
{
    return mDocDm->elementsByTagName("rules").item(0);
}

QDomNode
vleDmDD::nodeRule(const QString& ruleName) const
{
    QDomNode vars = nodeRules();
    return DomFunctions::childWhithNameAttr(vars , "rule", ruleName);
}

QDomNodeList
vleDmDD::predicatesFromDoc()
{
    return nodePreds().toElement().elementsByTagName("predicate");
}

QString
vleDmDD::getPredicateOperator(const QString& predicateName)
{
    if (not existPredToDoc(predicateName)) {
        return {};
    }

    QDomNode predicate = nodePred(predicateName);

    return DomFunctions::attributeValue(predicate, "operator");
}

QString
vleDmDD::getPredicateLeftType(const QString& predicateName)
{
    if (not existPredToDoc(predicateName)) {
        return {};
    }

    QDomNode predicate = nodePred(predicateName);

    return DomFunctions::attributeValue(predicate, "leftType");
}

QString
vleDmDD::getPredicateRightType(const QString& predicateName)
{
    if (not existPredToDoc(predicateName)) {
        return {};
    }

    QDomNode predicate = nodePred(predicateName);

    return DomFunctions::attributeValue(predicate, "rightType");
}

QString
vleDmDD::getPredicateRightValue(const QString& predicateName)
{
    if (not existPredToDoc(predicateName)) {
        return {};
    }

    QDomNode predicate = nodePred(predicateName);

    return DomFunctions::attributeValue(predicate, "rightValue");
}

QString
vleDmDD::getPredicateLeftValue(const QString& predicateName)
{
    if (not existPredToDoc(predicateName)) {
        return {};
    }

    QDomNode predicate = nodePred(predicateName);

    return DomFunctions::attributeValue(predicate, "leftValue");
}

void
vleDmDD::setPredicateOperator(const QString& predicateName,
                              const QString& op,
                              bool snap)
{
    if (not existPredToDoc(predicateName)) {
        return;
    }

    QDomNode pred = nodePred(predicateName);

    if (snap) {
        undoStackDm->snapshot(pred);
    }

    DomFunctions::setAttributeValue(pred, "operator", op);

    if (snap) {
        emit modified(PREDICATE);
    }
}

void
vleDmDD::setPredicateLeftType(const QString& predicateName,
                              const QString& lt,
                              bool snap)
{
    if (not existPredToDoc(predicateName)) {
        return;
    }

    QDomNode pred = nodePred(predicateName);

    if (snap) {
        undoStackDm->snapshot(pred);
    }

    DomFunctions::setAttributeValue(pred, "leftType", lt);

    if (snap) {
        emit modified(PREDICATE);
    }
}

void
vleDmDD::setPredicateRightType(const QString& predicateName,
                              const QString& rt,
                              bool snap)
{
    if (not existPredToDoc(predicateName)) {
        return;
    }

    QDomNode pred = nodePred(predicateName);

    if (snap) {
         undoStackDm->snapshot(pred);
    }

    DomFunctions::setAttributeValue(pred, "rightType", rt);

    if (snap) {
        emit modified(PREDICATE);
    }
}


void
vleDmDD::setPredicateRightValue(const QString& predicateName,
                                const QString& type,
                                const QString& orv,
                                const QString& rv,
                                bool snap)
{
    if (not existPredToDoc(predicateName)) {
        return;
    }

    if (type == "rename") {
        if (singleVarUsage(orv)){
            vleDomStatic::renamePortToInNode(nodeIn(), orv,
                                             rv, 0);
            vleDomStatic::renameObservablePort(nodeObs(), orv, rv, 0);

        } else {
            vleDomStatic::addPortToInNode(*mDocDm, nodeIn(), rv, 0);
            vleDomStatic::addObservablePort(*mDocDm, nodeObs(), rv, 0);

        }
    }

    if (type == "Val") {
        if (singleVarUsage(orv)){
            vleDomStatic::rmPortToInNode(nodeIn(), orv);
            vleDomStatic::rmObservablePort(nodeObs(), orv);
        }
    }

    QDomNode pred = nodePred(predicateName);

    if (snap) {
        undoStackDm->snapshot(pred);
    }

    DomFunctions::setAttributeValue(pred, "rightValue", rv);

    if (snap) {
        emit modified(PREDICATE);
    }
}

void
vleDmDD::setPredicateLeftValue(const QString& predicateName,
                               const QString& type,
                               const QString& olv,
                               const QString& lv,
                               bool snap)
{
    if (not existPredToDoc(predicateName)) {
        return;
    }

    if (type == "rename") {
        if (singleVarUsage(olv)){
            vleDomStatic::renamePortToInNode(nodeIn(), olv,
                                             lv, 0);
            vleDomStatic::renameObservablePort(nodeObs(), olv,
                                               lv, 0);
        } else {
            vleDomStatic::addPortToInNode(*mDocDm, nodeIn(), lv, 0);
            vleDomStatic::addObservablePort(*mDocDm, nodeObs(), lv, 0);
        }
    }

    if (type == "Val") {
        if (singleVarUsage(olv)){
            vleDomStatic::rmPortToInNode(nodeIn(), olv);
            vleDomStatic::rmObservablePort(nodeObs(), olv);
        }
    }

    QDomNode pred = nodePred(predicateName);

    if (snap) {
        undoStackDm->snapshot(pred);
    }

    DomFunctions::setAttributeValue(pred, "leftValue", lv);

    if (snap) {
        emit modified(PREDICATE);
    }
}

void
vleDmDD::addPredicateToDoc(const QString& predicateName)
{
    if (existPredToDoc(predicateName)) {
        return;
    }

    QDomNode rootNode = mDocDm->documentElement();
    QDomNode predsNode = mDocDm->elementsByTagName("predicates").item(0);

    undoStackDm->snapshot(rootNode);

    QDomElement xElem;
    //add predicate
    QDomElement el = mDocDm->createElement("predicate");
    el.setAttribute("name", predicateName);
    el.setAttribute("leftType", "Var");
    el.setAttribute("leftValue", "variableName");
    el.setAttribute("operator", "==");
    el.setAttribute("rightType", "Val");
    el.setAttribute("rightValue", "0.");
    predsNode.appendChild(el);

    //add Port
    vleDomStatic::addPortToInNode(*mDocDm, nodeIn(), "variableName", 0);
    vleDomStatic::addObservablePort(*mDocDm, nodeObs(), "variableName", 0);
    emit modified(OTHER);
}

QString
vleDmDD::newPredicateNameToDoc()
{
    return DomFunctions::childNameProvider(nodePreds(),"predicate",
            "predicate");
}

bool
vleDmDD::singleVarUsage(const QString& varName)
{
    QDomNodeList predList = predicatesFromDoc();
    int nbVarUsage = 0;
    for (int i = 0; i < predList.length(); i++) {
        QString left = predList.item(i).attributes().namedItem("leftValue").nodeValue();
        QString right = predList.item(i).attributes().namedItem("rightValue").nodeValue();
        if (left == varName) {
            nbVarUsage++;
        }
        if (right == varName) {
            nbVarUsage++;
        }

    }

    return (nbVarUsage == 1);

}

bool
vleDmDD::singleOutUsage(const QString& outName)
{
    QDomNodeList outList = nodeActs().toElement().elementsByTagName("outputParam");
    int nbOutUsage = 0;
    for (int i = 0; i < outList.length(); i++) {
        QString name = outList.item(i).attributes().namedItem("name").nodeValue();
        if (outName == name) {
            nbOutUsage++;
        }
    }
    return (nbOutUsage == 1);
}

bool
vleDmDD::outUsed(const QString& outName)
{
    QDomNodeList outList = nodeActs().toElement().elementsByTagName("outputParam");
    int nbOutUsage = 0;
    for (int i = 0; i < outList.length(); i++) {
        QString name = outList.item(i).attributes().namedItem("name").nodeValue();
        if (outName == name) {
            nbOutUsage++;
        }
    }
    return (nbOutUsage != 0);
}

bool
vleDmDD::existPredToDoc(const QString& predName)
{
    QDomNode pred = nodePred(predName);
    return not pred.isNull();
}

void
vleDmDD::rmPredicateToDoc(const QString& predName)
{
    if (not existPredToDoc(predName)) {
        return;
    }

    undoStackDm->snapshot(mDocDm->documentElement());



    QDomNode predsNode = nodePreds();
    QDomNode predNode = nodePred(predName);

    {
        QString type = predNode.attributes().namedItem("leftType").nodeValue();
        QString varName = predNode.attributes().namedItem("leftValue").nodeValue();

        if (type == "Var") {
            if (singleVarUsage(varName)) {
                vleDomStatic::rmPortToInNode(nodeIn(), varName);
                vleDomStatic::rmObservablePort(nodeObs(), varName);
            }
        }
    }
    {
        QString type = predNode.attributes().namedItem("rightType").nodeValue();
        QString varName = predNode.attributes().namedItem("rightValue").nodeValue();

        if (type == "Var") {
            if (singleVarUsage(varName)) {
                vleDomStatic::rmPortToInNode(nodeIn(), varName);
                vleDomStatic::rmObservablePort(nodeObs(), varName);
            }
        }
    }
    predsNode.removeChild(predNode);

    QSet<QString> rules = rulesNames();
    QSet<QString>::iterator itb = rules.begin();
    QSet<QString>::iterator ite = rules.end();
    for (; itb != ite; itb++) {
        QDomNode predicateRuleNode = nodePredicateRule(*itb, predName);
        if (not predicateRuleNode.isNull()){
            QDomNode rule = nodeRule(*itb);
            rule.removeChild(predicateRuleNode);
        }
    }
    emit modified(RENAMEPREDICATE);
}

void
vleDmDD::renamePredicateToDoc(const QString& oldName,
                              const QString& newName)
{
    QDomNode rootNode = mDocDm->documentElement();

    QDomNode pred = nodePred(oldName);
    if (pred.isNull()) {
        return ;
    }

    if (existPredToDoc(newName)) {
        emit modified(RENAMEPREDICATE);
        return;
    }

    undoStackDm->snapshot(rootNode);
    DomFunctions::setAttributeValue(pred, "name", newName);

    QSet<QString> rules = rulesNames();
    QSet<QString>::iterator itb = rules.begin();
    QSet<QString>::iterator ite = rules.end();
    for (; itb != ite; itb++) {
        QDomNode predicateRuleNode = nodePredicateRule(*itb, oldName);
        if (not predicateRuleNode.isNull()){
            DomFunctions::setAttributeValue(predicateRuleNode, "name", newName);
        }
    }

    emit modified(RENAMEPREDICATE);
}

QDomNode
vleDmDD::nodePreds() const
{
    return mDocDm->elementsByTagName("predicates").item(0);
}

QDomNode
vleDmDD::nodePred(const QString& predName) const
{
    QDomNode vars = nodePreds();
    return DomFunctions::childWhithNameAttr(vars , "predicate", predName);
}


void
vleDmDD::addActivityToDoc(const QString& actName, QPointF pos)
{
    if (existActToDoc(actName)) {
        return;
    }

    QDomNode rootNode = mDocDm->documentElement();
    QDomNode actsNode = mDocDm->elementsByTagName("activities").item(0);

    undoStackDm->snapshot(rootNode);

    QDomElement xElem;
    //add activity
    QDomElement el = mDocDm->createElement("activity");
    el.setAttribute("name", actName);
    el.setAttribute("value", pos.x());
    el.setAttribute("y", pos.y());
    el.setAttribute("minstart", "");
    el.setAttribute("maxfinish", "");
    el.setAttribute("maxiter", "1");
    el.setAttribute("timeLag", "1");
    xElem = mDocDm->createElement("outputParams");
    el.appendChild(xElem);
    xElem = mDocDm->createElement("rulesAssigment");
    el.appendChild(xElem);
    actsNode.appendChild(el);

    vleDomStatic::addObservablePort(*mDocDm, nodeObs(), "Activity_" + actName);
    vleDomStatic::addObservablePort(*mDocDm, nodeObs(), "Activity(state)_" + actName);
    vleDomStatic::addObservablePort(*mDocDm, nodeObs(), "Activity(ressources)_" + actName);

    emit modified(OTHER);
}

QDomNode
vleDmDD::nodeObs() const
{
    QDomNode obsNode = mDocDm->elementsByTagName("observable").item(0);
    return obsNode;
}

void
vleDmDD::setPositionToActivity(const QString& actName, QPointF pos,
        bool snap)
{
    if (not existActToDoc(actName)) {
        return;
    }

    QDomNode act = nodeAct(actName);
    if (snap) {
        vle::value::Map* forUndoMerge = new vle::value::Map();
        forUndoMerge->addString("query", mVdoDm->getXQuery(act).toStdString());
        undoStackDm->snapshot(act, "setPositionToActivity",
                forUndoMerge);
    }
    DomFunctions::setAttributeValue(act, "x", QVariant(pos.x()).toString());
    DomFunctions::setAttributeValue(act, "y", QVariant(pos.y()).toString());
    if (snap) {
        emit modified(MOVE_OBJ);
    }
}

void
vleDmDD::setMaxIter(const QString& actName, const int maxIter,
                    bool snap)
{
    if (not existActToDoc(actName)) {
        return;
    }

    QDomNode act = nodeAct(actName);
    if (snap) {
          undoStackDm->snapshot(act);
    }

    DomFunctions::setAttributeValue(act, "maxiter",  QString::number(maxIter));
    if (snap) {
        emit modified(PREDICATE);
    }
}

void
vleDmDD::setTimeLag(const QString& actName, const int timeLag,
                    bool snap)
{
    if (not existActToDoc(actName)) {
        return;
    }

    QDomNode act = nodeAct(actName);
    if (snap) {
          undoStackDm->snapshot(act);
    }

    DomFunctions::setAttributeValue(act, "timelag", QString::number(timeLag));
    if (snap) {
        emit modified(PREDICATE);
    }
}
void
vleDmDD::setMinStart(const QString& actName, const QString& min,
                     bool snap)
{
    if (not existActToDoc(actName)) {
        return;
    }

    QDomNode act = nodeAct(actName);
    if (snap) {
          undoStackDm->snapshot(act);
    }

    DomFunctions::setAttributeValue(act, "minstart", min);
    if (snap) {
        emit modified(PREDICATE);
    }
}

void
vleDmDD::setMaxFinish(const QString& actName, const QString& max,
		      bool snap)
{
    if (not existActToDoc(actName)) {
        return;
    }

    QDomNode act = nodeAct(actName);

    if (snap) {
         undoStackDm->snapshot(act);
    }

    DomFunctions::setAttributeValue(act, "maxfinish", max);

    if (snap) {
        emit modified(MOVE_OBJ);
    }
}

bool
vleDmDD::isRelativeDate(const QString& actName)
{
    return getMinStart(actName)[0] == '+' ||
        getMaxFinish(actName)[0] == '+';
}

int
vleDmDD::getTimeLag(const QString& actName)
{
    if (not existActToDoc(actName)) {
        return {};
    }

    QDomNode act = nodeAct(actName);

    return DomFunctions::attributeValue(act, "timelag").toInt();
}

int
vleDmDD::getMaxIter(const QString& actName)
{
    if (not existActToDoc(actName)) {
        return {};
    }

    QDomNode act = nodeAct(actName);

    return DomFunctions::attributeValue(act, "maxiter").toInt();
}
QString
vleDmDD::getMinStart(const QString& actName)
{
    if (not existActToDoc(actName)) {
        return {};
    }

    QDomNode act = nodeAct(actName);

    return DomFunctions::attributeValue(act, "minstart");
}

QString
vleDmDD::getMaxFinish(const QString& actName)
{
    if (not existActToDoc(actName)) {
        return {};
    }

    QDomNode act = nodeAct(actName);

    return DomFunctions::attributeValue(act, "maxfinish");
}

void
vleDmDD::renameActivityToDoc(const QString& oldName,
                             const QString& newName)
{
    QDomNode rootNode = mDocDm->documentElement();

    QDomNode act = nodeAct(oldName);
    if (act.isNull()) {
        return ;
    }

    undoStackDm->snapshot(rootNode);
    QDomNodeList precedences = PrecedencesFromDoc();

    for (int i = 0; i < precedences.length(); i++) {

        QDomNode precedence = precedences.item(i);
        {
            QString extremite = precedence.attributes().namedItem("first").nodeValue();
            if (extremite == oldName) {
                DomFunctions::setAttributeValue(precedence, "first", newName);
            }
        }
        {
            QString extremite = precedence.attributes().namedItem("second").nodeValue();
            if (extremite == oldName) {
                DomFunctions::setAttributeValue(precedence, "second", newName);
            }
        }
    }

    DomFunctions::setAttributeValue(act, "name", newName);

    vleDomStatic::renameObservablePort(nodeObs(), "Activity_" + oldName,
                                       "Activity_" + newName, 0);
    vleDomStatic::renameObservablePort(nodeObs(), "Activity(state)_" + oldName,
                                       "Activity(state)_" + newName, 0);
    vleDomStatic::renameObservablePort(nodeObs(), "Activity(ressources)_" + oldName,
                                        "Activity(ressources)_" + newName, 0);

    emit modified(RENAME);
}

QString
vleDmDD::newActivityNameToDoc()
{
    return DomFunctions::childNameProvider(nodeActs(),"activity",
            "activity");
}

bool
vleDmDD::existActToDoc(QString actName)
{
    QDomNode act = nodeAct(actName);
    return not act.isNull();
}

void
vleDmDD::rmActivityToDoc(const QString& actName)
{
    if (not existActToDoc(actName)) {
        return;
    }

    undoStackDm->snapshot(mDocDm->documentElement());

    rmPrecedencesToDoc(actName);

    QDomNode actssNode = nodeActs();
    QDomNode actNode = nodeAct(actName);
    actssNode.removeChild(actNode);

    vleDomStatic::rmObservablePort(nodeObs(), "Activity_" + actName);
    vleDomStatic::rmObservablePort(nodeObs(), "Activity(state)_" + actName);
    vleDomStatic::rmObservablePort(nodeObs(), "Activity(ressources)_" + actName);

    emit modified(RENAME);
}

QDomNodeList
vleDmDD::activitiesFromDoc()
{
    return nodeActs().toElement().elementsByTagName("activity");
}

QSet<QString>
vleDmDD::activityNames()
{
    return DomFunctions::childNames(nodeActs(), "activity");
}

QSet<QString>
vleDmDD::rulesNames()
{
    return DomFunctions::childNames(nodeRules(), "rule");
}

QSet<QString>
vleDmDD::predicatesNamesRule(QString ruleName)
{
    return DomFunctions::childNames(nodeRule(ruleName), "predicate");
}

QString
vleDmDD::getDataModel()
{
    QDomElement docElem = mDocDm->documentElement();

    QDomNode dataModelNode =
        mDocDm->elementsByTagName("dataModel").item(0);
    return dataModelNode.attributes().namedItem("conf").nodeValue();
}

void
vleDmDD::setCurrentTab(QString tabName)
{
    undoStackDm->current_source = tabName;
}

void
vleDmDD::setClassNameToDoc(const QString& className, bool snap)
{
    QDomNode rootNode = mDocDm->documentElement();

    if (snap) {
        undoStackDm->snapshot(rootNode);
    }

    QDomNode dynamicNode =
        mDocDm->elementsByTagName("dynamic").item(0);
    dynamicNode.toElement().setAttribute("name", "dyn" + className);
    dynamicNode.toElement().setAttribute("library", className);

    if (snap) {
        emit modified(OTHER);
    }
}
void
vleDmDD::setPluginNameToDoc(const QString& plugName, bool snap)
{
    QDomNode rootNode = mDocDm->documentElement();

    if (snap) {
        undoStackDm->snapshot(rootNode);
    }

    QDomNode dataModelNode =
        mDocDm->elementsByTagName("dataPlugin").item(0);
    dataModelNode.toElement().setAttribute("name", plugName);

    if (snap) {
        emit modified(OTHER);
    }
}

void
vleDmDD::setDataNameToDoc(const QString& dataName, const QString& pacNam, bool snap)
{
    QDomNode rootNode = mDocDm->documentElement();

    if (snap) {
        undoStackDm->snapshot(rootNode);
    }

    QDomNode dataModelNode =
        mDocDm->elementsByTagName("dataModel").item(0);
    dataModelNode.toElement().setAttribute("conf", dataName);

    QDomNode obsNode =
        mDocDm->elementsByTagName("observable").item(0);
    obsNode.toElement().setAttribute("name", "obs" + dataName);

    QDomNode condNode =
        mDocDm->elementsByTagName("condition").item(0);
    condNode.toElement().setAttribute("name", "cond" + dataName);

    vle::value::Map tofill;
    tofill.add("Rotation", vle::value::Map::create());
    tofill["Rotation"]->toMap().add("", vle::value::Set::create());
    tofill["Rotation"]->toMap()[""]->toSet().add(vle::value::Integer::create(1));
    tofill["Rotation"]->toMap()[""]->toSet().add(vle::value::Set::create());
    tofill["Rotation"]->toMap()[""]->toSet()[1]->toSet().add(vle::value::Integer::create(1));
    tofill["Rotation"]->toMap()[""]->toSet()[1]->toSet().add(vle::value::String::create(dataName.toStdString()));
    tofill.add("PlansLocation", vle::value::String::create((pacNam.toStdString())));

    vleDomStatic::fillConditionWithMap(*mDocDm, condNode, tofill);

    if (snap) {
        emit modified(OTHER);
    }
}

void
vleDmDD::setDataPackageToDoc(const QString& pacName, bool snap)
{
    QDomNode rootNode = mDocDm->documentElement();

    if (snap) {
        undoStackDm->snapshot(rootNode);
    }

    QDomNode dataModelNode =
        mDocDm->elementsByTagName("dataModel").item(0);
    dataModelNode.toElement().setAttribute("package", pacName);

    if (snap) {
        emit modified(OTHER);
    }
}

void
vleDmDD::setPackageToDoc(const QString& nm, bool snap)
{
    QDomNode rootNode = mDocDm->documentElement();
    if (snap) {
        undoStackDm->snapshot(rootNode);
    }

    QDomNode dynamicNode =
        mDocDm->elementsByTagName("dynamic").item(0);

    dynamicNode.toElement().setAttribute("package", nm);

    QDomNode dataModelNode =
        mDocDm->elementsByTagName("dataPlugin").item(0);

    dataModelNode.toElement().setAttribute("package", nm);

    if (snap) {
        emit modified(OTHER);
    }
}


void
vleDmDD::save()
{
    QFile file(mFileNameDm);
    QFileInfo fileInfo(file);
    if (not fileInfo.dir().exists()) {
        if (not QDir().mkpath(fileInfo.dir().path())) {
            return;
        }
    }
    if (not file.exists()) {
        if (not file.open(QIODevice::WriteOnly)) {
            return;
        }
        file.close();
    }
    file.open(QIODevice::Truncate | QIODevice::WriteOnly);
    QByteArray xml = mDocDm->toByteArray();
    file.write(xml);
    file.close();

    providePlan();

    undoStackDm->registerSaveState();
}

void
vleDmDD::providePlan()
{
     if (mFileNameSrc != "") {
        QFile filePlan(mFileNameSrc);
        if (filePlan.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
            filePlan.write(getData().toStdString().c_str()) ;
            filePlan.flush();
            filePlan.close();
        }
    }
}

QString
vleDmDD::getData()
{
    QString tpl =
        "#/**\n"                                                        \
        "#  * @file {{classname}}.txt\n"                                \
        "#  * @author ...\n"                                            \
        "#  * ...\n"                                                    \
        "#  */"                                                         \
        "\n\n"                                                          \
        "## Predicates\n"                                               \
        "{{predicatesOpenTag}}"                                         \
        "{{for i in predicatesList}}"                                   \
        "   predicate {\n"                                              \
        "      id = \"{{predicatesList^i}}\n"                           \
        "   }\n"                                                        \
        "{{end for}}"                                                   \
        "{{predicatesCloseTag}}"                                        \
        "## Rules\n"                                                    \
        "{{rulesOpenTag}}"                                              \
        "{{for i in rulesList}}"                                        \
        "   rule {\n"                                                   \
        "      id = \"{{rulesList^i}}\n"                                \
        "   }\n"                                                        \
        "{{end for}}"                                                   \
        "{{rulesCloseTag}}"                                             \
        "## Activities\n"                                               \
        "{{activitiesOpenTag}}"                                         \
        "{{for i in activitiesList}}"                                   \
        "   activity {\n"                                               \
        "      id = \"{{activitiesList^i}}\n"                           \
        "   }\n"                                                        \
        "{{end for}}"                                                   \
        "{{activitiesCloseTag}}"                                        \
        "## Precedences\n"                                              \
        "{{precedencesOpenTag}}"                                        \
        "{{for i in precedencesList}}"                                  \
        "   precedence {\n"                                             \
        "      type = {{precedencesList^i}}\n"                          \
        "   }\n"                                                        \
        "{{end for}}"                                                   \
        "{{precedencesCloseTag}}";


    QString predicatesOpenTag = "predicates {\n";
    QString rulesOpenTag = "rules {\n";
    QString activitiesOpenTag = "activities {\n";
    QString precedencesOpenTag = "precedences {\n";
    QString closeTag = "}\n\n";

    std::string esp16 = "            ";

    vle::utils::Template vleTpl(tpl.toStdString());
    vleTpl.stringSymbol().append("classname", getDataModel().toStdString());

    QDomNodeList actsXml = activitiesFromDoc();

    if (not actsXml.isEmpty()) {

        vleTpl.stringSymbol().append("activitiesOpenTag", activitiesOpenTag.toStdString());
        vleTpl.stringSymbol().append("activitiesCloseTag", closeTag.toStdString());

        vleTpl.listSymbol().append("activitiesList");

        QDomNodeList actsXml = activitiesFromDoc();
        for (int i = 0; i < actsXml.length(); i++) {

            QDomNode act = actsXml.item(i);
            QString name = act.attributes().namedItem("name").nodeValue();

            std::string actListElem = name.toStdString() + "\";\n";
            // temporal
            if (getMinStart(name) != "" ||
                getMaxFinish(name) != "") {
                actListElem =  actListElem + "      temporal {\n";
                if (getMinStart(name) != "" ) {
                    if (not isRelativeDate(name)) {
                        long numDate =
                            vle::utils::DateTime::toJulianDayNumber(getMinStart(name).toStdString());
                        actListElem =  actListElem +
                            esp16 + "minstart = " +
                            QString::number(numDate).toStdString() + "; # " +
                            getMinStart(name).toStdString() + "\n";
                    } else {
                        actListElem = actListElem +
                            esp16 + "minstart = \"" +
                            getMinStart(name).toStdString() + "\";\n";
                    }
                }
                if  ( getMaxFinish(name) != "" ) {
                    if (not isRelativeDate(name)) {
                        long numDate =
                            vle::utils::DateTime::toJulianDayNumber(getMaxFinish(name).toStdString());
                        actListElem =  actListElem +
                            esp16 + "maxfinish = " +
                            QString::number(numDate).toStdString() + "; #" +
                            getMaxFinish(name).toStdString() + "\n";
                    } else {
                        actListElem =  actListElem +
                            esp16 + "minstart = \"" +
                            getMaxFinish(name).toStdString() + "\";\n";
                    }
                }
                actListElem =  actListElem + "        }\n";
            }
            // generic output
            actListElem =  actListElem + "      output = \"GOut\";\n";
            // generic output
            actListElem =  actListElem + "      update = \"GUpdate\";\n";
            //Rules
            QDomNode rulesActNode = nodeRulesAct(name);
            QDomNodeList rules = rulesActNode.toElement().elementsByTagName("rule");
            if (rules.length() != 0) {
                actListElem += "      rules = ";
                QDomNode rule = rules.item(0);
                QString name = rule.attributes().namedItem("name").nodeValue();
                actListElem += "\"" + name.toStdString() + "\"";
                for (int i = 1 ; i < rules.length(); i++) {
                    rule = rules.item(i);
                    name = rule.attributes().namedItem("name").nodeValue();
                    actListElem += ", \"" + name.toStdString() + "\"";
                }
                actListElem += ";\n";
            }
            // parameter
            int maxIter = getMaxIter(name);
            QDomNodeList params = outParActFromDoc(name);
            if (params.length() != 0 or not (maxIter == 1)) {
                actListElem =  actListElem + "      parameter {\n";
                for (int i = 0; i < params.length(); i++) {
                    QDomNode variable = params.item(i);
                    QString name = variable.attributes().namedItem("name").nodeValue();
                    QString value = variable.attributes().namedItem("value").nodeValue();
                    actListElem =  actListElem +
                        esp16 + "_out_" + name.toStdString() + " = " + value.toStdString() + ";\n";
                }
                int timeLag = getTimeLag(name);
                if (not (maxIter == 1)) {
                    actListElem =  actListElem +
                        esp16 + "maxIter = " + QString::number(maxIter).toStdString() + ";\n";
                    actListElem =  actListElem +
                        esp16 + "timeLag = " + QString::number(timeLag).toStdString() + ";\n";
                }
                actListElem =  actListElem + "      }";
            }
            vleTpl.listSymbol().append("activitiesList", actListElem);
        }
    } else {
        vleTpl.stringSymbol().append("activitiesOpenTag", "");
        vleTpl.stringSymbol().append("activitiesCloseTag", "");
    }

    QDomNodeList predicatesXml = predicatesFromDoc();
    if (not predicatesXml.isEmpty()) {

        vleTpl.stringSymbol().append("predicatesOpenTag", predicatesOpenTag.toStdString());
        vleTpl.stringSymbol().append("predicatesCloseTag", closeTag.toStdString());

        for (int i = 0; i < predicatesXml.length(); i++) {

            QDomNode pred = predicatesXml.item(i);
            QString name = pred.attributes().namedItem("name").nodeValue();

            std::string predListElem = name.toStdString() + "\";\n";
            predListElem += "      type = \"GPred\";\n";
            predListElem += "      parameter {\n";
            predListElem += "         _opLeftType = \"" +
                pred.attributes().namedItem("leftType").nodeValue().toStdString() + "\";\n";
            if (pred.attributes().namedItem("leftType").nodeValue() == "Val") {
                predListElem += "         _opLeft = " +
                    pred.attributes().namedItem("leftValue").nodeValue().toStdString() + ";\n";

            } else {
                predListElem += "         _opLeft = \"" +
                    pred.attributes().namedItem("leftValue").nodeValue().toStdString() + "\";\n";
        }
            predListElem += "         _op = \"" +
                pred.attributes().namedItem("operator").nodeValue().toStdString() + "\";\n";
            predListElem += "         _opRightType = \"" +
                pred.attributes().namedItem("rightType").nodeValue().toStdString() + "\";\n";
            if (pred.attributes().namedItem("rightType").nodeValue() == "Val") {
                predListElem += "         _opRight = " +
                    pred.attributes().namedItem("rightValue").nodeValue().toStdString() + ";\n";
            } else {
                predListElem += "         _opRight = \"" +
                    pred.attributes().namedItem("rightValue").nodeValue().toStdString() + "\";\n";
            }
            predListElem += "      }";
            vleTpl.listSymbol().append("predicatesList", predListElem);
        }
    } else {
        vleTpl.stringSymbol().append("predicatesOpenTag", "");
        vleTpl.stringSymbol().append("predicatesCloseTag", "");
    }


    QDomNodeList rulesXml = RulesFromDoc();
    if (not rulesXml.isEmpty()) {

        vleTpl.stringSymbol().append("rulesOpenTag", rulesOpenTag.toStdString());
        vleTpl.stringSymbol().append("rulesCloseTag", closeTag.toStdString());

        for (int i = 0; i < rulesXml.length(); i++) {

            QDomNode rule = rulesXml.item(i);
            QString name = rule.attributes().namedItem("name").nodeValue();

            std::string ruleListElem = name.toStdString() + "\";\n";

            QDomNodeList preds = rule.toElement().elementsByTagName("predicate");
            if (preds.length() != 0) {
                ruleListElem += "      predicates = ";
                QDomNode pred = preds.item(0);
                QString name = pred.attributes().namedItem("name").nodeValue();
                ruleListElem += "\"" + name.toStdString() + "\"";
                for (int i = 1 ; i < preds.length(); i++) {
                    pred = preds.item(i);
                    name = pred.attributes().namedItem("name").nodeValue();
                    ruleListElem += ", \"" + name.toStdString() + "\"";
                }
                ruleListElem += ";";
            }
            vleTpl.listSymbol().append("rulesList", ruleListElem);
        }
    } else {
        vleTpl.stringSymbol().append("rulesOpenTag", "");
        vleTpl.stringSymbol().append("rulesCloseTag", "");
    }



    QDomNodeList precedencesXml = PrecedencesFromDoc();
    if (not precedencesXml.isEmpty()) {

        vleTpl.stringSymbol().append("precedencesOpenTag", precedencesOpenTag.toStdString());
        vleTpl.stringSymbol().append("precedencesCloseTag", closeTag.toStdString());
        for (int i = 0; i < precedencesXml.length(); i++) {

            QDomNode precedence = precedencesXml.item(i);
            QString type = precedence.attributes().namedItem("type").nodeValue();
            QString first = precedence.attributes().namedItem("first").nodeValue();
            QString second = precedence.attributes().namedItem("second").nodeValue();
            QString mintl = precedence.attributes().namedItem("mintimelag").nodeValue();
            QString maxtl = precedence.attributes().namedItem("maxtimelag").nodeValue();

            std::string precedenceListElem = type.toStdString() + ";\n";
            precedenceListElem += "      first = \"" + first.toStdString() + "\";\n";
            precedenceListElem += "      second = \"" + second.toStdString() + "\";\n";
            if (mintl != "") {
                precedenceListElem += "      mintimelag = " + mintl.toStdString() + ";\n";
            }
            if (maxtl != "") {
                precedenceListElem += "      maxtimelag = " + maxtl.toStdString() + ";\n";
            }

            precedenceListElem += "   }\n";
            vleTpl.listSymbol().append("precedencesList", precedenceListElem);
        }
    } else {
        vleTpl.stringSymbol().append("precedencesOpenTag", "");
        vleTpl.stringSymbol().append("precedencesCloseTag", "");
    }


    std::ostringstream out;
    vleTpl.process(out);

    return QString(out.str().c_str());
}

QDomNode
vleDmDD::nodeIn() const
{
    QDomNode inNode = mDocDm->elementsByTagName("in").item(0);
    return inNode;
}

QDomNode
vleDmDD::nodeOut() const
{
    QDomNode outNode = mDocDm->elementsByTagName("out").item(0);
    return outNode;
}

QDomNode
vleDmDD::nodeCond() const
{
    QDomNode condNode = mDocDm->elementsByTagName("condition").item(0);
    return condNode;
}

void
vleDmDD::undo()
{
    waitUndoRedoDm = true;
    undoStackDm->undo();
    emit modified(OTHER);

}

void
vleDmDD::redo()
{
    waitUndoRedoDm = true;
    undoStackDm->redo();
    emit modified(OTHER);
}

QDomNode
vleDmDD::nodeActs() const
{
    return mDocDm->elementsByTagName("activities").item(0);
}

QDomNode
vleDmDD::nodeAct(const QString& actName) const
{
    QDomNode vars = nodeActs();
    return DomFunctions::childWhithNameAttr(vars , "activity", actName);
}

void
vleDmDD::onUndoRedoDm(QDomNode oldDm, QDomNode newDm)
{
    oldValDm = oldDm;
    newValDm = newDm;
    waitUndoRedoDm = false;
    emit undoRedo(oldValDm, newValDm);
}

void
vleDmDD::onUndoAvailable(bool b)
{
    emit undoAvailable(b);
}

QDomNode
vleDmDD::nodeCondPort(const QString& portName) const
{
    QDomNode conditionNode =
            mDocDm->elementsByTagName("condition").item(0);
    QDomNodeList portList =
        conditionNode.toElement().elementsByTagName("port");
    for (int i = 0; i< portList.length(); i++) {
        QDomNode port = portList.at(i);
        for (int j=0; j< port.attributes().size(); j++) {
            if ((port.attributes().item(j).nodeName() == "name") and
                    (port.attributes().item(j).nodeValue() == portName))  {
                return port;
            }
        }
    }
    return QDomNode() ;
}


}}//namespaces
