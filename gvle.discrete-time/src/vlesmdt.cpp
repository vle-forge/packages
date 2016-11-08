/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2016 INRA http://www.inra.fr
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
#include <vle/value/Double.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/gvle/vlevpz.hpp>

#include "vlesmdt.h"

namespace vv = vle::value;

namespace vle {
namespace gvle {

vleDomSmDT::vleDomSmDT(QDomDocument* doc): DomObject(doc)

{

}

vleDomSmDT::~vleDomSmDT()
{

}

QString
vleDomSmDT::getXQuery(QDomNode node)
{
    QString name = node.nodeName();
    if ((name == "definition") or
        (name == "configuration") or
        (name == "compute") or
        (name == "constructor") or
        (name == "usersection") or
        (name == "includes") or
        (name == "variables") or
        (name == "srcPlugin") or
        (name == "dynamic") or
        (name == "observable") or
        (name == "condition") or
        (name == "in") or
        (name == "out")) {
        return getXQuery(node.parentNode())+"/"+name;
    }
    //element identified by attribute name
    if ((name == "variable")){
        return getXQuery(node.parentNode())+"/"+name+"[@name=\""
                +DomFunctions::attributeValue(node,"name")+"\"]";
    }
    if (node.nodeName() == "vle_project_metadata") {
        return "./vle_project_metadata";
    }
    return "";
}

QDomNode
vleDomSmDT::getNodeFromXQuery(const QString& query,
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
    if ((curr == "definition") or
        (curr == "configuration") or
        (curr == "variables") or
        (curr == "compute") or
        (curr == "constructor") or
        (curr == "usersection") or
        (curr == "includes") or
        (curr == "srcPlugin") or
        (curr == "dynamic") or
        (curr == "observable") or
        (curr == "condition") or
        (curr == "in") or
        (curr == "out")) {
        return getNodeFromXQuery(rest, DomFunctions::obtainChild(
                d, curr, mDoc));
    }
    //handle recursion with nodes identified by name
    std::vector<QString> nodeByNames;
    nodeByNames.push_back(QString("variable"));
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

vleSmDT::vleSmDT(const QString& srcpath, const QString& smpath,
        QString pluginName):
    mDocSm(0), mFileNameSrc(srcpath), mFileNameSm(smpath), mVdoSm(0),
    undoStackSm(0), waitUndoRedoSm(false),
    oldValSm(), newValSm(), mpluginName(pluginName)
{
    QFile file(mFileNameSm);
    if (file.exists()) {
        mDocSm = new QDomDocument("vle_project_metadata");
        QXmlInputSource source(&file);
        QXmlSimpleReader reader;
        mDocSm->setContent(&source, &reader);
    } else {
        xCreateDom();
    }

    mVdoSm = new vleDomSmDT(mDocSm);
    undoStackSm = new DomDiffStack(mVdoSm);
    undoStackSm->init(*mDocSm);

    QObject::connect(undoStackSm,
                SIGNAL(undoRedoVdo(QDomNode, QDomNode)),
                this, SLOT(onUndoRedoSm(QDomNode, QDomNode)));
    QObject::connect(undoStackSm,
                SIGNAL(undoAvailable(bool)),
                this, SLOT(onUndoAvailable(bool)));
}

void
vleSmDT::xCreateDom()
{
    if (not mDocSm) {
        mDocSm = new QDomDocument("vle_project_metadata");
        QDomProcessingInstruction pi;
        pi = mDocSm->createProcessingInstruction("xml",
                "version=\"1.0\" encoding=\"UTF-8\" ");
        mDocSm->appendChild(pi);

        QDomElement vpmRoot = mDocSm->createElement("vle_project_metadata");
        vpmRoot.setAttribute("version", "1.x");
        vpmRoot.setAttribute("author", "meto");

        QDomElement xPlug = mDocSm->createElement("srcPlugin");
        xPlug.setAttribute("name", mpluginName);
        vpmRoot.appendChild(xPlug);

        QDomElement xElem;

        QDomElement xDefModel = mDocSm->createElement("definition");
        xElem = mDocSm->createElement("variables");
        xDefModel.appendChild(xElem);
        xElem = mDocSm->createElement("compute");
        xDefModel.appendChild(xElem);
        xElem = mDocSm->createElement("constructor");
        xDefModel.appendChild(xElem);
        xElem = mDocSm->createElement("usersection");
        xDefModel.appendChild(xElem);
        xElem = mDocSm->createElement("includes");
        xDefModel.appendChild(xElem);
        vpmRoot.appendChild(xDefModel);

        QDomElement xConfModel = mDocSm->createElement("configuration");
        xElem = getDomDoc().createElement("dynamic");
        xConfModel.appendChild(xElem);
        xElem = getDomDoc().createElement("observable");
        xConfModel.appendChild(xElem);
        xElem = getDomDoc().createElement("condition");
        xConfModel.appendChild(xElem);
        xElem = getDomDoc().createElement("in");
        xConfModel.appendChild(xElem);
        xElem = getDomDoc().createElement("out");
        xConfModel.appendChild(xElem);
        vpmRoot.appendChild(xConfModel);

        mDocSm->appendChild(vpmRoot);
    }
}

void
vleSmDT::xReadDom()
{
    if (mDocSm) {
        QFile file(mFileNameSm);
        mDocSm->setContent(&file);
    }
}

void
vleSmDT::addVariableToDoc(const QString& varName, bool vect)
{
    if (existVarToDoc(varName)) {
        return;
    }

    QDomNode rootNode = mDocSm->documentElement();
    QDomNode variablesNode = mDocSm->elementsByTagName("variables").item(0);


    undoStackSm->snapshot(rootNode);

    QDomElement el = mDocSm->createElement("variable");
    el.setAttribute("name", varName);
    variablesNode.appendChild(el);
    if (vect) {
        DomFunctions::setAttributeValue(el, "type", "vect");
        setDim(varName, 2, false);
    } else {
        DomFunctions::setAttributeValue(el, "type", "var");
    }
    vleDomStatic::addObservablePort(*mDocSm, nodeObs(), varName);
    setType(varName, "Out", false);

    emit modified();
}

bool
vleSmDT::isVect(const QString& varName) const
{
    QDomNode var = nodeVariable(varName);
    if (var.isNull()) {
        return false;
    }
    bool res = (DomFunctions::attributeValue(var, "type") == "vect");
    return res;
}

bool
vleSmDT::isIn(const QString& varName) const
{
    QDomNode inNode =
        mDocSm->elementsByTagName("in").item(0);

    QDomNode port = DomFunctions::childWhithNameAttr(inNode, "port", varName);
    return not port.isNull();
}

bool
vleSmDT::isOut(const QString& varName) const
{
    QDomNode inNode = mDocSm->elementsByTagName("out").item(0);
    QDomNode port = DomFunctions::childWhithNameAttr(inNode, "port", varName);
    return not port.isNull();
}

QString
vleSmDT::getType(const QString& varName) const
{
    QString type = "Out";
    if (isIn(varName)) {
        if (isOut(varName)) {
            type = "In/Out";
        } else {
            type = "In";
        }
    }
    return type;
}

void
vleSmDT::setType(const QString& varName, const QString& type, bool snap)
{
    QDomNode inNode = nodeIn();
    QDomNode outNode = nodeOut();
    QDomNode configNode = nodeConfiguration();
    if (snap) {
        undoStackSm->snapshot(configNode);
    }
    if (type == "In") {
        vleDomStatic::rmPortToOutNode(outNode, varName, 0);
        vleDomStatic::addPortToInNode(*mDocSm, inNode, varName, 0);
    } else if (type == "Out") {
        vleDomStatic::rmPortToInNode(inNode, varName, 0);
        vleDomStatic::addPortToOutNode(*mDocSm, outNode, varName, 0);
        QDomNode cond = nodeCond();
        vleDomStatic::rmPortFromCond(cond, "sync_"+varName, 0);
    } else if (type == "In/Out") {
        vleDomStatic::addPortToInNode(*mDocSm, inNode, varName, 0);
        vleDomStatic::addPortToOutNode(*mDocSm, outNode, varName, 0);
    }
    if (snap) {
        emit modified();
    }

}


bool
vleSmDT::isObs(const QString& varName) const
{
    QDomNode inNode = mDocSm->elementsByTagName("observable").item(0);
    QDomNode port = DomFunctions::childWhithNameAttr(inNode, "port", varName);
    return not port.isNull();
}

void
vleSmDT::setInitialValue(const QString& varName,
                         const vv::Value& val,
                         bool snap)
{
    if (not existVarToDoc(varName)) {
        return;
    }

    QDomNode cond = nodeCond();

    if (snap) {
        undoStackSm->snapshot(cond);
    }
    vle::value::Map initPort;
    initPort.add("init_value_"+varName.toStdString(), val.clone());
    vleDomStatic::fillConditionWithMap(*mDocSm, cond, initPort, undoStackSm);

    if (snap)  {
        emit modified();
    }
}

void
vleSmDT::setTimeStep(vv::Double& val)
{
    QDomNode condNode =
        mDocSm->elementsByTagName("condition").item(0);

    undoStackSm->snapshot(condNode);

    QDomNode port = nodeCondPort("time_step");
    if (port.isNull()) {
        QDomElement el = mDocSm->createElement("port");
        el.setAttribute("name", "time_step");
        condNode.appendChild(el);
        port = nodeCondPort("time_step");
    }

    DomFunctions::removeAllChilds(port);
    QDomElement vale = mDocSm->createElement("double");
    vleDomStatic::fillWithValue(*mDocSm, vale, val);
    port.appendChild(vale);

    emit modified();
}

bool
vleSmDT::hasTimeStep()
{
    return not nodeCondPort("time_step").isNull();
}

double
vleSmDT::getTimeStep()
{
    QDomNode port = nodeCondPort("time_step");
    if (port.isNull()) {
        return 0;
    }
    QDomNode init_value = port.firstChildElement();
    return vleDomStatic::buildValue(init_value, false)->toDouble().value();
}

void
vleSmDT::UnSetTimeStep()
{
    QDomNode condNode =
        mDocSm->elementsByTagName("condition").item(0);

    undoStackSm->snapshot(condNode);

    QDomNode port = nodeCondPort("time_step");
    if (port.isNull()) {
        return;
    }

    condNode.removeChild(port);

    emit modified();
}

void
vleSmDT::setHistorySize(const QString& varName, int hsize, bool snap)
{
    if (hsize < 1) {
        return;
    }
    QDomNode cond = nodeCond();
    std::unique_ptr<value::Value> oldHsize =
            vleDomStatic::getValueFromPortCond(cond,"history_size_"+varName, 0);
    std::unique_ptr<value::Value> initVal =
            vleDomStatic::getValueFromPortCond(cond,"init_value_"+varName, 0);
    std::unique_ptr<value::Value> dim =
            vleDomStatic::getValueFromPortCond(cond,"dim_"+varName, 0);
    if (isVect(varName)) {
        if (dim and initVal) {
            initVal->toTable().resize(hsize, dim->toInteger().value());

        }
    } else {
        if (initVal) {
            int oldHsizeVal = 1;
            if (oldHsize) {
                oldHsizeVal = oldHsize->toInteger().value();
            }
            if (oldHsizeVal == 1) {
                if (hsize > 1) {
                    initVal = vle::value::Tuple::create(hsize,
                            initVal->toDouble().value());
                }
            } else {
                if (hsize == 1) {
                    initVal = vle::value::Double::create(
                            initVal->toTuple().at(0));
                } else {
                    initVal->toTuple().resize(hsize);
                }
            }
        }
    }

    vle::value::Map initPort;
    initPort.addInt("history_size_"+varName.toStdString(), hsize);
    if (initVal) {
        initPort.add("init_value_"+varName.toStdString(), std::move(initVal));
    }
    if (snap) {
        undoStackSm->snapshot(cond);
    }
    vleDomStatic::fillConditionWithMap(*mDocSm, cond, initPort);
    if (snap) {
        emit modified();
    }
}

void
vleSmDT::setDim(const QString& varName, int dim, bool snap)
{
    if (not isVect(varName)) {
        return;
    }
    QDomNode cond = nodeCond();
    if (snap) {
        undoStackSm->snapshot(cond);
    }
    vle::value::Map initPort;
    initPort.addInt("dim_"+varName.toStdString(), dim);
    std::unique_ptr<vle::value::Value> val = getInitialValue(varName);
    if (val) {
        vle::value::Table& valT = val->toTable();
        valT.resize(valT.width(), dim);
        initPort.add("init_value_"+varName.toStdString(), std::move(val));
    }
    vleDomStatic::fillConditionWithMap(*mDocSm, cond, initPort);
    if (snap) {
        emit modified();
    }
}

void
vleSmDT::setSync(const QString& varName, int val)
{
    QDomNode cond = nodeCond();
    vle::value::Map initPort;
    std::string portName = "sync_"+varName.toStdString();
    initPort.addInt(portName, val);
    vleDomStatic::fillConditionWithMap(*mDocSm, cond, initPort, undoStackSm);
}

bool
vleSmDT::hasInitialValue(const QString& varName) const
{
    QDomNode cond = nodeCond();
    QString init_name = "init_value_" + varName;
    if (not vleDomStatic::existPortFromCond(cond, init_name)) {
        return false;
    } else {
        return true;
    }
}

std::unique_ptr<value::Value>
vleSmDT::getInitialValue(const QString& varName) const
{
    QDomNode cond = nodeCond();
    QString init_name = "init_value_" + varName;
    if (not vleDomStatic::existPortFromCond(cond, init_name)) {
        return nullptr;
    }
    QDomNode port = nodeCondPort("init_value_" + varName);
    if (port.isNull()) {
        return std::unique_ptr<value::Value>();
    }
    QDomNode init_value = port.firstChildElement();
    return std::move(vleDomStatic::buildValue(init_value, false));
}

void
vleSmDT::setInitialDefaultValue(const QString& varName)
{
    if (not existVarToDoc(varName)) {
        return;
    }
    QDomNode cond = nodeCond();
    int h = getHistorySize(varName);
    int d = getDim(varName);

    vle::value::Map initPort;
    std::unique_ptr<vle::value::Value> val;

    if (isVect(varName)) {
        val = vle::value::Table::create(h, d);
    } else if (h > 1) {
        val = vle::value::Tuple::create(h);
    } else {
        val = vle::value::Double::create(0);
    }
    initPort.add("init_value_"+varName.toStdString(), std::move(val));

    vleDomStatic::fillConditionWithMap(*mDocSm, cond, initPort, undoStackSm);

    emit modified();

}

void
vleSmDT::unsetInitialValue(const QString& varName)
{
    if (not existVarToDoc(varName)) {
        return;
    }
    QDomNode cond = nodeCond();
    undoStackSm->snapshot(cond);

    QString portName = "init_value_"+varName;
    vleDomStatic::rmPortFromCond(cond, portName);
    emit modified();
}

int
vleSmDT::getHistorySize(const QString& varName)
{
    QDomNode cond = nodeCond();
    std::unique_ptr<value::Value> hsize = vleDomStatic::getValueFromPortCond(
            cond,"history_size_"+varName, 0);
    if (hsize) {
        return hsize->toInteger().value();
    }
    return 1;
}

int
vleSmDT::getDim(const QString& varName)
{
    QDomNode var = nodeVariable(varName);
    if (var.isNull()) {
        return -1;
    }
    QDomNode cond = nodeCond();
    std::unique_ptr<value::Value> dim = vleDomStatic::getValueFromPortCond(
            cond, "dim_"+varName, 0);
    if (dim) {
        return dim->toInteger().value();
    }
    return 1;
}

int
vleSmDT::getSync(const QString& varName)
{
    QDomNode port = nodeCondPort("sync_" + varName);
    if (port.isNull()) {
        return -1;
    }

    QDomNode syncValue= port.toElement().elementsByTagName("integer").at(0);
    if (syncValue.nodeName() == "integer") {
        QVariant qv = syncValue.childNodes().item(0).toText().nodeValue();;
        return qv.toInt();
    }
    return 0;
}

void
vleSmDT::addObsToDoc(const QString& varName)
{
    QDomNode obsNode =
        mDocSm->elementsByTagName("observable").item(0);
    undoStackSm->snapshot(obsNode);
    vleDomStatic::addObservablePort(*mDocSm, obsNode, varName);
    emit modified();
}


void
vleSmDT::rmObsToDoc(const QString& varName)
{
    QDomNode obsNode =
        mDocSm->elementsByTagName("observable").item(0);
    undoStackSm->snapshot(obsNode);
    vleDomStatic::rmObservablePort(obsNode, varName);
    emit modified();
}

void
vleSmDT::rmVariableToDoc(const QString& varName)
{
    if (not existVarToDoc(varName)) {
        return;
    }
    undoStackSm->snapshot(mDocSm->documentElement());

    QDomNode varsNode = nodeVariables();
    QDomNode varNode = nodeVariable(varName);
    varsNode.removeChild(varNode);

    vleDomStatic::rmPortToInNode(nodeIn(),varName);
    vleDomStatic::rmPortToOutNode(nodeOut(),varName);
    vleDomStatic::rmObservablePort(nodeObs(), varName);
    vleDomStatic::rmPortFromCond(nodeCond(), "init_value_"+varName);
    vleDomStatic::rmPortFromCond(nodeCond(), "sync_"+varName);
    vleDomStatic::rmPortFromCond(nodeCond(), "dim_"+varName);


    emit modified();
}


void
vleSmDT::renameVariableToDoc(const QString& oldVariableName,
        const QString& newVariableName)
{
    QDomNode rootNode = mDocSm->documentElement();

    QDomNode var = nodeVariable(oldVariableName);
    if (var.isNull()) {
        return ;
    }

    undoStackSm->snapshot(rootNode);

    DomFunctions::setAttributeValue(var, "name", newVariableName);

    vleDomStatic::renamePortFromCond(nodeCond(), "init_value_"+oldVariableName,
            "init_value_"+newVariableName, 0);
    vleDomStatic::renamePortFromCond(nodeCond(), "sync_"+oldVariableName,
            "sync_"+newVariableName, 0);
    vleDomStatic::renamePortFromCond(nodeCond(), "dim_"+oldVariableName,
            "dim_"+newVariableName, 0);
    vleDomStatic::renameObservablePort(nodeObs(), oldVariableName,
            newVariableName, 0);
    vleDomStatic::renamePortToOutNode(nodeOut(), oldVariableName,
            newVariableName, 0);
    vleDomStatic::renamePortToInNode(nodeIn(), oldVariableName,
            newVariableName, 0);

    emit modified();
}

QString
vleSmDT::newVarNameToDoc()
{
    QString varName = "NewVar";
    unsigned int idVar = 0;
    bool varNameFound = false;
    while (not varNameFound) {
        if (existVarToDoc(varName)) {
            idVar ++;
            varName = "NewVar";
            varName += "_";
            varName += QVariant(idVar).toString();
        } else {
            varNameFound = true;
        }
    }
    return varName;
}

bool
vleSmDT::existVarToDoc(QString varName)
{
    QDomNode var = nodeVariable(varName);
    return not var.isNull();
}

void
vleSmDT::setComputeToDoc(const QString& computeBody)
{
    QDomElement docElem = mDocSm->documentElement();

    QDomNode computeNode =
            mDocSm->elementsByTagName("compute").item(0);

    undoStackSm->snapshot(computeNode);

    QDomNodeList childs = computeNode.childNodes();
    if (childs.length() == 0) {
        QDomCDATASection cdataCompute = mDocSm->createCDATASection(computeBody);
        computeNode.appendChild(cdataCompute);
    } else {
        QDomCDATASection cdataCompute = childs.at(0).toCDATASection();
        cdataCompute.setData(computeBody);
    }

    emit modified();
}

QString
vleSmDT::getComputeBody()
{
    QDomElement docElem = mDocSm->documentElement();

    QDomNode computeNode =
        mDocSm->elementsByTagName("compute").item(0);

    QDomNodeList childs = computeNode.childNodes();
    if (childs.length() == 0) {
        return "";
    } else {
        QDomCDATASection cdataCompute = childs.at(0).toCDATASection();
        return cdataCompute.data();
    }
}

void
vleSmDT::setConstructorToDoc(const QString& constructorBody)
{
    QDomElement docElem = mDocSm->documentElement();

    QDomNode constructorNode =
            mDocSm->elementsByTagName("constructor").item(0);

    undoStackSm->snapshot(constructorNode);

    QDomNodeList childs = constructorNode.childNodes();
    if (childs.length() == 0) {
        QDomCDATASection cdataConstructor = mDocSm->createCDATASection(constructorBody);
        constructorNode.appendChild(cdataConstructor);
    } else {
        QDomCDATASection cdataConstructor = childs.at(0).toCDATASection();
        cdataConstructor.setData(constructorBody);
    }

    emit modified();
}

QString
vleSmDT::getConstructorBody()
{
    QDomElement docElem = mDocSm->documentElement();

    QDomNode constructorNode =
        mDocSm->elementsByTagName("constructor").item(0);

    QDomNodeList childs = constructorNode.childNodes();
    if (childs.length() == 0) {
        return "";
    } else {
        QDomCDATASection cdataConstructor = childs.at(0).toCDATASection();
        return cdataConstructor.data();
    }
}

void
vleSmDT::setUserSectionToDoc(const QString& usersectionBody)
{
    QDomElement docElem = mDocSm->documentElement();

    QDomNode usersectionNode =
            mDocSm->elementsByTagName("usersection").item(0);

    undoStackSm->snapshot(usersectionNode);

    QDomNodeList childs = usersectionNode.childNodes();
    if (childs.length() == 0) {
        QDomCDATASection cdataUserSection = mDocSm->createCDATASection(usersectionBody);
        usersectionNode.appendChild(cdataUserSection);
    } else {
        QDomCDATASection cdataUserSection = childs.at(0).toCDATASection();
        cdataUserSection.setData(usersectionBody);
    }

    emit modified();
}

QString
vleSmDT::getUserSectionBody()
{
    QDomElement docElem = mDocSm->documentElement();

    QDomNode usersectionNode =
        mDocSm->elementsByTagName("usersection").item(0);

    QDomNodeList childs = usersectionNode.childNodes();
    if (childs.length() == 0) {
        return "";
    } else {
        QDomCDATASection cdataUserSection = childs.at(0).toCDATASection();
        return cdataUserSection.data();
    }
}

void
vleSmDT::setIncludesToDoc(const QString& includesBody)
{
    QDomElement docElem = mDocSm->documentElement();

    QDomNode includesNode =
            mDocSm->elementsByTagName("includes").item(0);

    undoStackSm->snapshot(includesNode);

    QDomNodeList childs = includesNode.childNodes();
    if (childs.length() == 0) {
        QDomCDATASection cdataIncludes = mDocSm->createCDATASection(includesBody);
        includesNode.appendChild(cdataIncludes);
    } else {
        QDomCDATASection cdataIncludes = childs.at(0).toCDATASection();
        cdataIncludes.setData(includesBody);
    }

    emit modified();
}

QString
vleSmDT::getIncludesBody()
{
    QDomElement docElem = mDocSm->documentElement();

    QDomNode includesNode =
        mDocSm->elementsByTagName("includes").item(0);

    QDomNodeList childs = includesNode.childNodes();
    if (childs.length() == 0) {
        return "";
    } else {
        QDomCDATASection cdataIncludes = childs.at(0).toCDATASection();
        return cdataIncludes.data();
    }
}

QString
vleSmDT::getSrcPlugin()
{
    QDomElement docElem = mDocSm->documentElement();

    QDomNode srcPluginNode =
        mDocSm->elementsByTagName("srcPlugin").item(0);
    return srcPluginNode.attributes().namedItem("name").nodeValue();
}

void
vleSmDT::setClassNameToDoc(const QString& className, bool snap)
{
    QDomNode rootNode = mDocSm->documentElement();

    if (snap) {
        undoStackSm->snapshot(rootNode);
    }


    QDomNode srcPluginNode =
        mDocSm->elementsByTagName("srcPlugin").item(0);
    srcPluginNode.toElement().setAttribute("class", className);

    QDomNode dynamicNode =
        mDocSm->elementsByTagName("dynamic").item(0);
    dynamicNode.toElement().setAttribute("name", "dyn" + className);
    dynamicNode.toElement().setAttribute("library", className);

    QDomNode obsNode =
        mDocSm->elementsByTagName("observable").item(0);
    obsNode.toElement().setAttribute("name", "obs" + className);

    QDomNode condNode =
        mDocSm->elementsByTagName("condition").item(0);
    condNode.toElement().setAttribute("name", "cond" + className);

    if (snap) {
        emit modified();
    }
}

QString
vleSmDT::getClassName()
{
    QDomElement docElem = mDocSm->documentElement();

    QDomNode srcPluginNode =
        mDocSm->elementsByTagName("srcPlugin").item(0);
    return srcPluginNode.attributes().namedItem("class").nodeValue();
}

void
vleSmDT::setPackageToDoc(const QString& nm, bool snap)
{
    QDomNode rootNode = mDocSm->documentElement();
    if (snap) {
        undoStackSm->snapshot(rootNode);
    }

    QDomNode srcPluginNode =
        mDocSm->elementsByTagName("srcPlugin").item(0);

    srcPluginNode.toElement().setAttribute("package", nm);

    QDomNode dynamicNode =
        mDocSm->elementsByTagName("dynamic").item(0);

    dynamicNode.toElement().setAttribute("package", nm);
    if (snap) {
        emit modified();
    }
}

QString
vleSmDT::getPackage()
{
    QDomElement docElem = mDocSm->documentElement();

    QDomNode srcPluginNode =
        mDocSm->elementsByTagName("srcPlugin").item(0);
    return srcPluginNode.attributes().namedItem("package").nodeValue();
}

QDomNodeList
vleSmDT::variablesFromDoc()
{
    QDomElement docElem = mDocSm->documentElement();
    QDomNode variablesNode =
        mDocSm->elementsByTagName("variables").item(0);
    return variablesNode.toElement().elementsByTagName("variable");
}

void
vleSmDT::setCurrentTab(QString tabName)
{
    undoStackSm->current_source = tabName;
}

void
vleSmDT::save()
{
    QFile file(mFileNameSm);
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
    QByteArray xml = mDocSm->toByteArray();
    file.write(xml);
    file.close();

    provideCpp();

    undoStackSm->registerSaveState();
}

void
vleSmDT::provideCpp()
{
    if (mFileNameSrc != "") {
        QFile fileCpp(mFileNameSrc);
        if (fileCpp.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
            fileCpp.write(getData().toStdString().c_str()) ;
            fileCpp.flush();
            fileCpp.close();
        }
    }
}

void
vleSmDT::undo()
{
    waitUndoRedoSm = true;
    undoStackSm->undo();
    emit modified();
}

void
vleSmDT::redo()
{
    waitUndoRedoSm = true;
    undoStackSm->redo();
    emit modified();
}

QString
vleSmDT::getData()
{
    QString tpl = "/**\n"                                               \
        "  * @file {{classname}}.cpp\n"                                 \
        "  * @author ...\n"                                             \
        "  * ...\n"                                                     \
        "  */\n\n"                                                      \
        "/*\n"                                                          \
        " * @@tagdynamic@@\n"                                           \
        " * @@tagdepends: vle.discrete-time @@endtagdepends\n"          \
        "*/\n\n"                                                        \
        "#include <vle/DiscreteTime.hpp>\n"                             \
        "{{includes}}\n"                                                \
        "namespace vd = vle::devs;\n\n"                                 \
        "namespace vv = vle::value;\n\n"                                \
        "namespace vle {\n"                                             \
        "namespace discrete_time {\n"                                   \
        "namespace {{namespace}} {\n\n"                                 \
        "class {{classname}} : public DiscreteTimeDyn\n"                \
        "{\n"                                                           \
        "public:\n"                                                     \
        "{{classname}}(\n"                                              \
        "    const vd::DynamicsInit& init,\n"                           \
        "    const vd::InitEventList& evts)\n"                          \
        "        : DiscreteTimeDyn(init, evts)\n"                       \
        "{\n"                                                           \
        "{{for i in var}}"                                              \
        "    {{var^i}}.init(this, \"{{var^i}}\", evts);\n"              \
        "{{end for}}"                                                   \
        "{{for i in vect}}"                                             \
        "    {{vect^i}}.init(this, \"{{vect^i}}\", evts);\n"            \
        "{{end for}}"                                                   \
        "{{construct}}\n"                                               \
        "\n"                                                            \
        "}\n"                                                           \
        "\n"                                                            \
        "virtual ~{{classname}}()\n"                                    \
        "{}\n"                                                          \
        "\n"                                                            \
        "void compute(const vle::devs::Time& t)\n"                      \
        "{\n"                                                           \
        "{{compute}}\n"                                                 \
        "}\n"                                                           \
        "\n"                                                            \
        "{{for i in var}}"                                              \
        "    Var {{var^i}};\n"                                          \
        "{{end for}}"                                                   \
        "{{for i in vect}}"                                             \
        "    Vect {{vect^i}};\n"                                        \
        "{{end for}}"                                                   \
        "{{userSection}}\n"                                             \
        "};\n\n"                                                        \
        "} // namespace {{namespace}}\n"                                \
        "} // namespace discrete_time\n"                                \
        "} // namespace vle\n\n"                                        \
        "DECLARE_DYNAMICS("                                             \
        "vle::discrete_time::{{namespace}}::{{classname}})\n\n";

    vle::utils::Template vleTpl(tpl.toStdString());
    vleTpl.stringSymbol().append("classname", getClassName().toStdString());
    vleTpl.stringSymbol().append("namespace",
            getPackage().replace('.', '_').toStdString());

    vleTpl.listSymbol().append("var");
    vleTpl.listSymbol().append("vect");


    QDomNodeList variablesXml = variablesFromDoc();
    for (int i = 0; i < variablesXml.length(); i++) {
        QDomNode variable = variablesXml.item(i);
        QString varName = variable.attributes().namedItem("name").nodeValue();
        if (isVect(varName)) {
            vleTpl.listSymbol().append("vect", varName.toStdString());
        } else {
            vleTpl.listSymbol().append("var", varName.toStdString());
        }
    }

    vleTpl.stringSymbol().append("includes", getIncludesBody().toStdString());
    vleTpl.stringSymbol().append("compute", getComputeBody().toStdString());
    vleTpl.stringSymbol().append("construct", getConstructorBody().toStdString());
    vleTpl.stringSymbol().append("userSection", getUserSectionBody().toStdString());

    std::ostringstream out;
    vleTpl.process(out);

    return QString(out.str().c_str());
}


QDomNode
vleSmDT::nodeVariables() const
{
    return mDocSm->elementsByTagName("variables").item(0);
}

QDomNode
vleSmDT::nodeVariable(const QString& varName) const
{
    QDomNode vars = nodeVariables();
    return DomFunctions::childWhithNameAttr(vars , "variable", varName);
}


QDomNode
vleSmDT::nodeCond() const
{
    QDomNode conditionNode = mDocSm->elementsByTagName("condition").item(0);
    return conditionNode;
}

QDomNode
vleSmDT::nodeCondPort(const QString& portName) const
{
    QDomNode conditionNode =
            mDocSm->elementsByTagName("condition").item(0);
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

QDomNode
vleSmDT::nodeObs() const
{
    QDomNode conditionNode = mDocSm->elementsByTagName("observable").item(0);
    return conditionNode;
}

QDomNode
vleSmDT::nodeObsPort(const QString& portName)
{
    QDomNode observableNode =
            mDocSm->elementsByTagName("observable").item(0);
    QDomNodeList obsList =
        observableNode.toElement().elementsByTagName("port");
    for (int i = 0; i< obsList.length(); i++) {
        QDomNode obs = obsList.at(i);
        for (int j=0; j< obs.attributes().size(); j++) {
            if ((obs.attributes().item(j).nodeName() == "name") and
                    (obs.attributes().item(j).nodeValue() == portName))  {
                return obs;
            }
        }
    }
    return QDomNode() ;
}

QDomNode
vleSmDT::nodeIn() const
{
    QDomNode inNode = mDocSm->elementsByTagName("in").item(0);
    return inNode;
}

QDomNode
vleSmDT::nodeOut() const
{
    QDomNode outNode = mDocSm->elementsByTagName("out").item(0);
    return outNode;
}

QDomNode
vleSmDT::nodeConfiguration() const
{
    QDomNode outNode = mDocSm->elementsByTagName("configuration").item(0);
    return outNode;
}

QDomNode
vleSmDT::nodeInPort(const QString& portName)
{
    QDomNode inNode =
            mDocSm->elementsByTagName("in").item(0);
    QDomNodeList inList =
        inNode.toElement().elementsByTagName("port");
    for (int i = 0; i< inList.length(); i++) {
        QDomNode in = inList.at(i);
        for (int j=0; j< in.attributes().size(); j++) {
            if ((in.attributes().item(j).nodeName() == "name") and
                    (in.attributes().item(j).nodeValue() == portName))  {
                return in;
            }
        }
    }
    return QDomNode() ;
}

QDomNode
vleSmDT::nodeOutPort(const QString& portName)
{
    QDomNode outNode =
            mDocSm->elementsByTagName("out").item(0);
    QDomNodeList outList =
        outNode.toElement().elementsByTagName("port");
    for (int i = 0; i< outList.length(); i++) {
        QDomNode out = outList.at(i);
        for (int j=0; j< out.attributes().size(); j++) {
            if ((out.attributes().item(j).nodeName() == "name") and
                    (out.attributes().item(j).nodeValue() == portName))  {
                return out;
            }
        }
    }
    return QDomNode() ;
}

void
vleSmDT::onUndoRedoSm(QDomNode oldSm, QDomNode newSm)
{
    oldValSm = oldSm;
    newValSm = newSm;
    waitUndoRedoSm = false;
    emit undoRedo(oldValSm, newValSm);
}

void
vleSmDT::onUndoAvailable(bool b)
{
    emit undoAvailable(b);
}

}
}
