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

#include <QFileInfo>
#include <QDir>
#include <QtXml/QDomNode>

#include <vle/utils/Template.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/gvle/vlevpz.h>

#include "vlesmdt.h"

namespace vv = vle::value;

namespace vle {
namespace gvle {

vleDomSmDT::vleDomSmDT(QDomDocument* doc): vleDomObject(doc)

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
                +attributeValue(node,"name")+"\"]";
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
        return getNodeFromXQuery(rest, obtainChild(d, curr, true));
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
        QDomNode selMod = childWhithNameAttr(d, selNodeByName, currSplit.at(1));
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
    undoStackSm = new vleDomDiffStack(mVdoSm);
    undoStackSm->init(*mDocSm);

    QObject::connect(undoStackSm,
                SIGNAL(undoRedoVdo(QDomNode, QDomNode)),
                this, SLOT(onUndoRedoSm(QDomNode, QDomNode)));
    QObject::connect(undoStackSm,
                SIGNAL(undoAvailable(bool)),
                this, SLOT(onUndoAvailable(bool)));
}

QString
vleSmDT::toQString(const QDomNode& node) const
{
    QString str;
    QTextStream stream(&str);
    node.save(stream, node.nodeType());
    return str;
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
vleSmDT::addVariableToDoc(const QString& varName)
{
    QDomNode rootNode = mDocSm->documentElement();

    QDomNode variablesNode =
            mDocSm->elementsByTagName("variables").item(0);
    if (existVarToDoc(varName)) {
        return;
    }

    undoStackSm->snapshot(rootNode);

    QDomElement el = mDocSm->createElement("variable");
    el.setAttribute("name", varName);
    variablesNode.appendChild(el);

    setHistorySize(varName, vv::Integer(0), false);
    setDim(varName, vv::Integer(1), false);

    QDomNode obsNode =
        mDocSm->elementsByTagName("observable").item(0);
    el = mDocSm->createElement("port");
    el.setAttribute("name", varName);
    obsNode.appendChild(el);

    QDomNode condNode =
        mDocSm->elementsByTagName("condition").item(0);
    el = mDocSm->createElement("port");
    el.setAttribute("name", "init_value_" + varName);
    condNode.appendChild(el);
    el = mDocSm->createElement("port");
    el.setAttribute("name", "sync_" + varName);
    condNode.appendChild(el);

    setPortCondDoubleValue("init_value_" + varName, vv::Double(0.0));
    setPortCondIntegerValue("sync_" + varName, vv::Integer(0.0));

    QDomNode inNode =
        mDocSm->elementsByTagName("in").item(0);
    el = mDocSm->createElement("port");
    el.setAttribute("name", varName);
    inNode.appendChild(el);

    QDomNode outNode =
        mDocSm->elementsByTagName("out").item(0);
    el = mDocSm->createElement("port");
    el.setAttribute("name", varName);
    outNode.appendChild(el);

    emit modified();
}

void
vleSmDT::setInitialDefValue(const QString& varName,
                            const vv::Value& val,
                            const bool snap)
{
    QDomNode var = nodeVariable(varName);
    if (var.isNull()) {
        return;
    }

    if (snap) {
        undoStackSm->snapshot(var);
    }


    QDomNode nodeValue = mDocSm->createElement("initial_value");
    QString tagName = "";
    switch (val.getType()) {
    case vv::Value::DOUBLE:
        tagName = "double";
        break;
    case vv::Value::TUPLE:
        tagName = "tuple";
        break;
    case vv::Value::MATRIX:
        tagName = "matrix";
        break;
    case vv::Value::BOOLEAN:
    case vv::Value::INTEGER:
    case vv::Value::STRING:
    case vv::Value::SET:
    case vv::Value::MAP:
    case vv::Value::TABLE:
    case vv::Value::XMLTYPE:
    case vv::Value::NIL:
    case vv::Value::USER:
        break;
    }

    QDomNodeList initList = var.toElement().elementsByTagName("initial_value");
    if (initList.length() == 0) {
        QDomNode child = mDocSm->createElement(tagName);
        vleVpz::fillWithValue(*mDocSm, mVdoSm, child, val);
        nodeValue.appendChild(child);
        var.appendChild(nodeValue);
    } else {
        QDomNode init_value = initList.at(0);
        vleVpz::removeAllChilds(init_value);
        QDomNode child = mDocSm->createElement(tagName);
        vleVpz::fillWithValue(*mDocSm, mVdoSm, child, val);
        init_value.appendChild(child);
    }

    if (snap) {
        emit modified();
    }
}

void
vleSmDT::setInitialCondValue(const QString& varName,
                             const vv::Value& val,
                             const bool snap)
{

    QDomNode condNode = mDocSm->elementsByTagName("condition").item(0);

    if (snap) {
        undoStackSm->snapshot(condNode);
    }

    QDomNode var = nodeVariable(varName);
    if (var.isNull()) {
        return;
    }

    QDomNode nodeValue = mDocSm->createElement("initial_value");
    QString tagName = "";
    switch (val.getType()) {
    case vv::Value::DOUBLE:
        tagName = "double";
        break;
    case vv::Value::TUPLE:
        tagName = "tuple";
        break;
    case vv::Value::MATRIX:
        tagName = "matrix";
        break;
    case vv::Value::BOOLEAN:
    case vv::Value::INTEGER:
    case vv::Value::STRING:
    case vv::Value::SET:
    case vv::Value::MAP:
    case vv::Value::TABLE:
    case vv::Value::XMLTYPE:
    case vv::Value::NIL:
    case vv::Value::USER:
        break;
    }

    QDomNode port = nodeCondPort("init_value_" + varName);
    if (port.isNull()) {
        QDomElement el = mDocSm->createElement("port");
        el.setAttribute("name", "init_value_" + varName);
        condNode.appendChild(el);
        port = nodeCondPort("init_value_" + varName);
    }

    vleVpz::removeAllChilds(port);
    QDomElement vale = mDocSm->createElement(tagName);
    vleVpz::fillWithValue(*mDocSm, mVdoSm, vale, val);
    port.appendChild(vale);

     if (snap) {
         emit modified();
     }
}

void
vleSmDT::setInitialValue(const QString& varName,
                         const vv::Value& val,
                         const bool snap)
{
    if (isParametrable(varName)) {
        setInitialCondValue(varName, val, snap);
    } else {
        setInitialDefValue(varName, val, snap);
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

    vleVpz::removeAllChilds(port);
    QDomElement vale = mDocSm->createElement("double");
    vleVpz::fillWithValue(*mDocSm, mVdoSm, vale, val);
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
    return vleVpz::buildValue(mVdoSm, init_value, false)->toDouble().value();
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
vleSmDT::setHistorySize(const QString& varName,
                        const vv::Value& val,
                        const bool snap)
{
    QDomNode var = nodeVariable(varName);
    if (var.isNull()) {
        return;
    }

    if (snap) {
        undoStackSm->snapshot(var);
    }

    QDomNodeList initList = var.toElement().elementsByTagName("history_size");
    if (initList.length() == 0) {
        //TODO see vpz management of vle values
        QDomElement dble = mDocSm->createElement("integer");
        dble.appendChild(mDocSm->createTextNode(
                val.writeToString().c_str()));
        QDomElement el = mDocSm->createElement("history_size");
        el.appendChild(dble);
        var.appendChild(el);
    } else {
        QDomNode histNode = initList.at(0);
        QDomNode histValue = histNode.toElement().elementsByTagName("integer").at(0);
        if (histValue.nodeName() == "integer") {
            QDomText dbleval = histValue.childNodes().item(0).toText();
            dbleval.setData(val.writeToString().c_str());
        }
    }

    if (snap) {
        emit modified();
    }
}

void
vleSmDT::setHistorySizeAndValue(const QString& varName,
                                const vv::Value& histValue,
                                const bool snap)
{
    if (not isParametrable(varName)) {
        QDomNode rootNode = mDocSm->documentElement();
        if (snap) {
            undoStackSm->snapshot(rootNode);
        }
        Parametrable(varName, true, false);
    } else {
        QDomNode condNode = mDocSm->elementsByTagName("condition").item(0);
        if (snap) {
            undoStackSm->snapshot(condNode);
        }
    }

    setHistorySize(varName, histValue, false);
    vv::Value* val = getInitialValue(varName);

    int historySize =  getHistorySize(varName)->toInteger().value();
    int dim =  getDim(varName)->toInteger().value();

    if (dim == 1) { // history is not managed by conds for vectors
        if (val) {
            int lastFeed = 0;
            if (historySize > 0) {
                std::unique_ptr<vv::Value> history = vv::Tuple::create();
                if (val->isDouble()) {
                    history->toTuple().add(val->toDouble().value());
                    lastFeed = 1;
                } else {
                    uint j = 0;
                    while (lastFeed <= historySize &&
                           j < val->toTuple().size()) {
                        history->toTuple().add(val->toTuple()[j]);
                        lastFeed++;
                        j++;
                    }
                }
                while (lastFeed <= historySize) {
                    history->toTuple().add(0.0);
                    lastFeed++;
                }
                setInitialValue(varName, *history, false);
            } else {
                if (val->isTuple()) {
                    setInitialValue(varName,
                                    vv::Double(val->toTuple()[0]),
                                    false);
                }
            }
        }
    }
    if (snap) {
        emit modified();
    }
}

void
vleSmDT::setDim(const QString& varName,
                const vv::Value& val,
                const bool snap)
{
    QDomNode var = nodeVariable(varName);
    if (var.isNull()) {
        return;
    }

    if (snap) {
        undoStackSm->snapshot(var);
    }

    QDomNodeList initList = var.toElement().elementsByTagName("dim");
    if (initList.length() == 0) {
        QDomElement dble = mDocSm->createElement("integer");
        dble.appendChild(mDocSm->createTextNode(
                val.writeToString().c_str()));
        QDomElement el = mDocSm->createElement("dim");
        el.appendChild(dble);
        var.appendChild(el);
    } else {
        QDomNode dimNode = initList.at(0);
        QDomNode dimValue= dimNode.toElement().elementsByTagName("integer").at(0);
        if (dimValue.nodeName() == "integer") {
            QDomText dbleval = dimValue.childNodes().item(0).toText();
            dbleval.setData(val.writeToString().c_str());
        }
    }

    if (snap) {
        emit modified();
    }
}

void
vleSmDT::setDimAndValue(const QString& varName,
                        const vv::Value& dimValue,
                        const bool snap)
{
    if (not isParametrable(varName)) {
        QDomNode rootNode = mDocSm->documentElement();
        if (snap) {
            undoStackSm->snapshot(rootNode);
        }
        Parametrable(varName, true, false);
    } else {
        QDomNode condNode = mDocSm->elementsByTagName("condition").item(0);
        if (snap) {
            undoStackSm->snapshot(condNode);
        }
    }

    setDim(varName, dimValue, false);
    vv::Value* val = getInitialValue(varName);

    int historySize =  getHistorySize(varName)->toInteger().value();
    int dim =  getDim(varName)->toInteger().value();
    if (dim == 1) {
        if (val) {
            int lastFeed = 0;
            if (historySize > 0) {
                std::unique_ptr<vv::Value> init = vv::Tuple::create();
                init->toTuple().add(val->toTuple()[0]);
                lastFeed = 1;
                while (lastFeed <= historySize) {
                    init->toTuple().add(0.0);
                    lastFeed++;
                }
                setInitialValue(varName, *init, false);
            } else {
                if (val->isTuple()) {
                    setInitialValue(varName,
                                    vv::Double(val->toTuple()[0]),
                                    false);
                }
            }
        }
    } else {
        if (val) {
            int lastFeed = 0;
            std::unique_ptr<vv::Value> init = vv::Tuple::create();
            if (val->isDouble()) {
                init->toTuple().add(val->toDouble().value());
                lastFeed = 1;
            } else {
                uint j = 0;
                while (lastFeed < dim &&
                       j < val->toTuple().size()) {
                    init->toTuple().add(val->toTuple()[j]);
                    lastFeed++;
                    j++;
                }
            }
            while (lastFeed < dim) {
                init->toTuple().add(0.0);
                lastFeed++;
            }
            setInitialValue(varName, *init, false);
        }
    }

    if (snap) {
        emit modified();
    }
}

void
vleSmDT::Parametrable(const QString& variableName,
                      const bool parametrable,
		      const bool snap)
{
    vv::Value* val = vleSmDT::getInitialValue(variableName);

    if ((getDim(variableName))->toInteger().value() > 1 or
        (getHistorySize(variableName))->toInteger().value() > 1) {
        return;
    }

    QDomNode rootNode = mDocSm->documentElement();
    if (snap) {
        undoStackSm->snapshot(rootNode);
    }

    if (parametrable) {
        QDomNode var = nodeVariable(variableName);
        QDomNodeList initList = var.toElement().elementsByTagName("initial_value");
        if (not initList.length() == 0) {
            QDomNode init_value = initList.at(0);
            vleVpz::removeAllChilds(init_value);
        }
        setInitialCondValue(variableName, *val, false);
    } else {
        QDomNode condNode = mDocSm->elementsByTagName("condition").item(0);
        QDomNode port = nodeCondPort("init_value_" + variableName);

        condNode.removeChild(port);

        setInitialDefValue(variableName, *val, false);
    }

    if (snap) {
        emit modified();
    }
}

void
vleSmDT::setSync(const QString& variableName,
		 const vv::Value& val)
{
    QDomNode port = nodeCondPort("sync_" + variableName);

    undoStackSm->snapshot(port);

    setPortCondIntegerValue("sync_" + variableName, val);
}

void
vleSmDT::setPortCondTupleValue(const QString& portName,
                               const vv::Value& val)
{
    QDomNode port = nodeCondPort(portName);
    if (port.isNull()) {
        return;
    }

    vleVpz::removeAllChilds(port);

    QDomElement tple = mDocSm->createElement("tuple");
    vleVpz::fillWithValue(*mDocSm, mVdoSm, tple, val);

    port.appendChild(tple);

    emit modified();
}

void
vleSmDT::setPortCondDoubleValue(const QString& portName,
                                const vv::Value& val)
{
    QDomNode port = nodeCondPort(portName);
    if (port.isNull()) {
        return;
    }

    vleVpz::removeAllChilds(port);

    QDomElement dble = mDocSm->createElement("double");
    dble.appendChild(mDocSm->createTextNode(
                         val.writeToString().c_str()));
    port.appendChild(dble);

    emit modified();
}

void
vleSmDT::setPortCondIntegerValue(const QString& portName,
                                 const vv::Value& val)
{
    QDomNode port = nodeCondPort(portName);
    if (port.isNull()) {
        return;
    }

    vleVpz::removeAllChilds(port);

    QDomElement dble = mDocSm->createElement("integer");
    dble.appendChild(mDocSm->createTextNode(
                         val.writeToString().c_str()));
    port.appendChild(dble);

    emit modified();
}

void
vleSmDT::setPortCondBoolValue(const QString& portName,
                              const vv::Value& val)
{
    QDomNode port = nodeCondPort(portName);
    if (port.isNull()) {
        return;
    }

    QDomElement dble = mDocSm->createElement("boolean");
    dble.appendChild(mDocSm->createTextNode(
                         val.writeToString().c_str()));
    port.appendChild(dble);

    emit modified();
}

vv::Value*
vleSmDT::getInitialValue(const QString& varName)
{
    if (isParametrable(varName)){
        QDomNode port = nodeCondPort("init_value_" + varName);
        if (port.isNull()) {
            return 0;
        }
        QDomNode init_value = port.firstChildElement();
        return vleVpz::buildValue(mVdoSm, init_value, false);
    } else {
        QDomNode var = nodeVariable(varName);
        if (var.isNull()) {
            return 0;
        }
        QDomNodeList initList = var.toElement().elementsByTagName("initial_value");
        if (initList.length() == 0) {
            return 0;
        }
        QDomNode init_value = initList.at(0).firstChildElement();
        return vleVpz::buildValue(mVdoSm, init_value, false);
    }
}

vv::Value*
vleSmDT::getHistorySize(const QString& varName)
{
    QDomNode var = nodeVariable(varName);
    if (var.isNull()) {
        return 0;
    }
    QDomNodeList initList = var.toElement().elementsByTagName("history_size");
    if (initList.length() == 0) {
        return 0;
    }

    QDomNode histNode = initList.at(0);
    QDomNode histValue = histNode.toElement().elementsByTagName("integer").at(0);
    if (histValue.nodeName() == "integer") {
        QVariant qv = histValue.childNodes().item(0).toText().nodeValue();;
        return new vv::Integer(qv.toInt());
    }
    return 0;
}

vv::Value*
vleSmDT::getDim(const QString& varName)
{
    QDomNode var = nodeVariable(varName);
    if (var.isNull()) {
        return 0;
    }
    QDomNodeList initList = var.toElement().elementsByTagName("dim");
    if (initList.length() == 0) {
        return 0;
    }
    QDomNode dimNode = initList.at(0);
    QDomNode dimValue= dimNode.toElement().elementsByTagName("integer").at(0);
    if (dimValue.nodeName() == "integer") {
        QVariant qv = dimValue.childNodes().item(0).toText().nodeValue();;
        return new vv::Integer(qv.toInt());
    }
    return 0;
}

vv::Value*
vleSmDT::getSync(const QString& varName)
{
    QDomNode port = nodeCondPort("sync_" + varName);
    if (port.isNull()) {
        return 0;
    }

    QDomNode syncValue= port.toElement().elementsByTagName("integer").at(0);
    if (syncValue.nodeName() == "integer") {
        QVariant qv = syncValue.childNodes().item(0).toText().nodeValue();;
        return new vv::Integer(qv.toInt());
    }
    return 0;
}

void
vleSmDT::addInToDoc(const QString& varName)
{
    QDomNode inNode =
        mDocSm->elementsByTagName("in").item(0);

    undoStackSm->snapshot(inNode);

     QDomElement el = mDocSm->createElement("port");
    el.setAttribute("name", varName);
    inNode.appendChild(el);

    emit modified();
}


void
vleSmDT::rmInToDoc(const QString& variableName)
{
    QDomNode inNode =
        mDocSm->elementsByTagName("in").item(0);

    undoStackSm->snapshot(inNode);

    QDomNodeList inPortList =
        inNode.toElement().elementsByTagName("port");

    for (int i = 0; i< inPortList.length(); i++) {
        QDomNode inPort = inPortList.at(i);
        for (int j = 0; j < inPort.attributes().size(); j++) {
            if ((inPort.attributes().item(j).nodeName() == "name") and
                (inPort.attributes().item(j).nodeValue() == variableName))  {

                inNode.removeChild(inPort);
            }
        }
    }
    emit  modified();
}

bool
vleSmDT::hasInFromDoc(const QString& variableName)
{
    QDomNode inNode =
        mDocSm->elementsByTagName("in").item(0);
    QDomNodeList inPortList =
        inNode.toElement().elementsByTagName("port");

    for (int i = 0; i< inPortList.length(); i++) {
        QDomNode inPort = inPortList.at(i);
        for (int j = 0; j < inPort.attributes().size(); j++) {
            if ((inPort.attributes().item(j).nodeName() == "name") and
                (inPort.attributes().item(j).nodeValue() == variableName))  {
                return true;
            }
        }
    }
    return false;
}

void
vleSmDT::addOutToDoc(const QString& varName)
{
    QDomNode inNode =
        mDocSm->elementsByTagName("out").item(0);

    undoStackSm->snapshot(inNode);

     QDomElement el = mDocSm->createElement("port");
    el.setAttribute("name", varName);
    inNode.appendChild(el);

    emit modified();
}


void
vleSmDT::rmOutToDoc(const QString& variableName)
{
    QDomNode inNode =
        mDocSm->elementsByTagName("out").item(0);

    undoStackSm->snapshot(inNode);

    QDomNodeList inPortList =
        inNode.toElement().elementsByTagName("port");

    for (int i = 0; i< inPortList.length(); i++) {
        QDomNode inPort = inPortList.at(i);
        for (int j = 0; j < inPort.attributes().size(); j++) {
            if ((inPort.attributes().item(j).nodeName() == "name") and
                (inPort.attributes().item(j).nodeValue() == variableName))  {

                inNode.removeChild(inPort);
            }
        }
    }
    emit  modified();
}

bool
vleSmDT::hasOutFromDoc(const QString& variableName)
{
    QDomNode inNode =
        mDocSm->elementsByTagName("out").item(0);
    QDomNodeList inPortList =
        inNode.toElement().elementsByTagName("port");

    for (int i = 0; i< inPortList.length(); i++) {
        QDomNode inPort = inPortList.at(i);
        for (int j = 0; j < inPort.attributes().size(); j++) {
            if ((inPort.attributes().item(j).nodeName() == "name") and
                (inPort.attributes().item(j).nodeValue() == variableName))  {
                return true;
            }
        }
    }
    return false;
}

void
vleSmDT::addObsToDoc(const QString& varName)
{
    QDomNode inNode =
        mDocSm->elementsByTagName("observable").item(0);

    undoStackSm->snapshot(inNode);

     QDomElement el = mDocSm->createElement("port");
    el.setAttribute("name", varName);
    inNode.appendChild(el);

    emit modified();
}


void
vleSmDT::rmObsToDoc(const QString& variableName)
{
    QDomNode inNode =
        mDocSm->elementsByTagName("observable").item(0);

    undoStackSm->snapshot(inNode);

    QDomNodeList inPortList =
        inNode.toElement().elementsByTagName("port");

    for (int i = 0; i< inPortList.length(); i++) {
        QDomNode inPort = inPortList.at(i);
        for (int j = 0; j < inPort.attributes().size(); j++) {
            if ((inPort.attributes().item(j).nodeName() == "name") and
                (inPort.attributes().item(j).nodeValue() == variableName))  {

                inNode.removeChild(inPort);
            }
        }
    }
    emit  modified();
}

bool
vleSmDT::hasObsFromDoc(const QString& variableName)
{
    QDomNode inNode =
        mDocSm->elementsByTagName("observable").item(0);
    QDomNodeList inPortList =
        inNode.toElement().elementsByTagName("port");

    for (int i = 0; i< inPortList.length(); i++) {
        QDomNode inPort = inPortList.at(i);
        for (int j = 0; j < inPort.attributes().size(); j++) {
            if ((inPort.attributes().item(j).nodeName() == "name") and
                (inPort.attributes().item(j).nodeValue() == variableName))  {
                return true;
            }
        }
    }
    return false;
}

bool
vleSmDT::isParametrable(const QString& variableName)
{
    if ((getDim(variableName))->toInteger().value() > 1 or
        (getHistorySize(variableName))->toInteger().value() > 1) {
        return true;
    } else {
        QDomNode port = nodeCondPort("init_value_" + variableName);
        return not port.isNull();
    }
}

void
vleSmDT::rmVariableToDoc(const QString& variableName)
{
    QDomNode rootNode = mDocSm->documentElement();

    undoStackSm->snapshot(rootNode);

    QDomNode variablesNode =
            mDocSm->elementsByTagName("variables").item(0);

    QDomNodeList variableList =
        variablesNode.toElement().elementsByTagName("variable");

    for (int i = 0; i< variableList.length(); i++) {
        QDomNode variable = variableList.at(i);
        for (int j = 0; j < variable.attributes().size(); j++) {
            if ((variable.attributes().item(j).nodeName() == "name") and
                (variable.attributes().item(j).nodeValue() == variableName))  {

                variablesNode.removeChild(variable);
            }
        }
    }

    QDomNode obsNode =
        mDocSm->elementsByTagName("observable").item(0);

    QDomNodeList obsPortList =
        obsNode.toElement().elementsByTagName("port");

    for (int i = 0; i< obsPortList.length(); i++) {
        QDomNode obsPort = obsPortList.at(i);
        for (int j = 0; j < obsPort.attributes().size(); j++) {
            if ((obsPort.attributes().item(j).nodeName() == "name") and
                (obsPort.attributes().item(j).nodeValue() == variableName))  {

                obsNode.removeChild(obsPort);
            }
        }
    }

    QDomNode condNode =
        mDocSm->elementsByTagName("condition").item(0);

    QDomNodeList condPortList =
        condNode.toElement().elementsByTagName("port");

    for (int i = 0; i< condPortList.length(); i++) {
        QDomNode condPort = condPortList.at(i);
        for (int j = 0; j < condPort.attributes().size(); j++) {
            if ((condPort.attributes().item(j).nodeName() == "name") and
                (condPort.attributes().item(j).nodeValue() ==
                 "init_value_" + variableName))  {

                condNode.removeChild(condPort);
            }
        }
    }

    condPortList =
        condNode.toElement().elementsByTagName("port");

    for (int i = 0; i< condPortList.length(); i++) {
        QDomNode condPort = condPortList.at(i);
        for (int j = 0; j < condPort.attributes().size(); j++) {
            if ((condPort.attributes().item(j).nodeName() == "name") and
                 (condPort.attributes().item(j).nodeValue() ==
                  "sync_" + variableName))  {

                condNode.removeChild(condPort);
            }
        }
    }

    QDomNode inNode =
        mDocSm->elementsByTagName("in").item(0);
    QDomNodeList inPortList =
        inNode.toElement().elementsByTagName("port");

    for (int i = 0; i< inPortList.length(); i++) {
        QDomNode inPort = inPortList.at(i);
        for (int j = 0; j < inPort.attributes().size(); j++) {
            if ((inPort.attributes().item(j).nodeName() == "name") and
                (inPort.attributes().item(j).nodeValue() == variableName))  {

                inNode.removeChild(inPort);
            }
        }
    }

    QDomNode outNode =
        mDocSm->elementsByTagName("out").item(0);
    QDomNodeList outPortList =
        outNode.toElement().elementsByTagName("port");

    for (int i = 0; i< outPortList.length(); i++) {
        QDomNode outPort = outPortList.at(i);
        for (int j = 0; j < outPort.attributes().size(); j++) {
            if ((outPort.attributes().item(j).nodeName() == "name") and
                    (outPort.attributes().item(j).nodeValue() == variableName))  {

                outNode.removeChild(outPort);
            }
        }
    }

    emit modified();
}


void
vleSmDT::renameVariableToDoc(const QString& oldVariableName,
        const QString& newVariableName)
{
    QDomNode rootNode = mDocSm->documentElement();

    QDomNode variablesNode =
            mDocSm->elementsByTagName("variables").item(0);

    QDomNode newvar = nodeVariable(newVariableName);
    if (not newvar.isNull()) {
        return ;
    }

    QDomNode var = nodeVariable(oldVariableName);
    if (var.isNull()) {
        return ;
    }

    undoStackSm->snapshot(rootNode);

    for (int j=0; j< var.attributes().size(); j++) {
        if (var.attributes().item(j).nodeName() == "name")  {
            var.attributes().item(j).setNodeValue(newVariableName);
        }
    }

    {
        QDomNode cond = nodeCondPort("init_value_" + oldVariableName);
        for (int j=0; j< cond.attributes().size(); j++) {
            if (cond.attributes().item(j).nodeName() == "name")  {
                cond.attributes().item(j).setNodeValue("init_value_" +
                                                       newVariableName);
            }
        }
    }

    {
        QDomNode cond = nodeCondPort("sync_" + oldVariableName);
        for (int j=0; j< cond.attributes().size(); j++) {
            if (cond.attributes().item(j).nodeName() == "name")  {
                cond.attributes().item(j).setNodeValue("sync_" +
                                                       newVariableName);
            }
        }
    }

    QDomNode obs = nodeObsPort(oldVariableName);
    for (int j=0; j< obs.attributes().size(); j++) {
        if (obs.attributes().item(j).nodeName() == "name")  {
            obs.attributes().item(j).setNodeValue(newVariableName);
        }
    }

    QDomNode in = nodeInPort(oldVariableName);
    for (int j=0; j< in.attributes().size(); j++) {
        if (in.attributes().item(j).nodeName() == "name")  {
            in.attributes().item(j).setNodeValue(newVariableName);
        }
    }

    QDomNode out = nodeOutPort(oldVariableName);
    for (int j=0; j< out.attributes().size(); j++) {
        if (out.attributes().item(j).nodeName() == "name")  {
            out.attributes().item(j).setNodeValue(newVariableName);
        }
    }

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
vleSmDT::setClassNameToDoc(const QString& className)
{
    QDomNode rootNode = mDocSm->documentElement();

    undoStackSm->snapshot(rootNode);

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

    emit modified();
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
vleSmDT::setPackageToDoc(const QString& nm)
{
    QDomNode rootNode = mDocSm->documentElement();
    undoStackSm->snapshot(rootNode);

    QDomNode srcPluginNode =
        mDocSm->elementsByTagName("srcPlugin").item(0);

    srcPluginNode.toElement().setAttribute("package", nm);

    QDomNode dynamicNode =
        mDocSm->elementsByTagName("dynamic").item(0);

    dynamicNode.toElement().setAttribute("package", nm);

    emit modified();
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

    undoStackSm->clear();
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
        "{{for i in init_value_val}}"                                   \
        "    {{init_value_var^i}}.init_value({{init_value_val^i}});\n"  \
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
    vleTpl.listSymbol().append("init_value_var");
    vleTpl.listSymbol().append("init_value_val");


    QDomNodeList variablesXml = variablesFromDoc();
    for (int i = 0; i < variablesXml.length(); i++) {
        QDomNode variable = variablesXml.item(i);
        QString varName = variable.attributes().namedItem("name").nodeValue();
        vv::Value* val = getInitialValue(varName);
        if (val) {
            if (not isParametrable(varName)) {
                vleTpl.listSymbol().append("init_value_var", varName.toStdString());
                vleTpl.listSymbol().append("init_value_val", val->writeToString());
            }
            delete val;
        }

        if ((getDim(varName))->toInteger().value() == 1) {
            vleTpl.listSymbol().append("var", varName.toStdString());
        } else {
            vleTpl.listSymbol().append("vect", varName.toStdString());
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
vleSmDT::nodeVariable(const QString& varName)
{
    QDomNode variablesNode =
            mDocSm->elementsByTagName("variables").item(0);
    QDomNodeList variableList =
            variablesNode.toElement().elementsByTagName("variable");
    for (int i = 0; i< variableList.length(); i++) {
        QDomNode variable = variableList.at(i);
        for (int j=0; j< variable.attributes().size(); j++) {
            if ((variable.attributes().item(j).nodeName() == "name") and
                    (variable.attributes().item(j).nodeValue() == varName))  {
                return variable;
            }
        }
    }
    return QDomNode() ;
}

QDomNode
vleSmDT::nodeCondPort(const QString& portName)
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
