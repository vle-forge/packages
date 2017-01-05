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

#include "vlesmForrester.h"



namespace gvle {
namespace forrester {

using namespace vle::gvle;
namespace vv = vle::value;

vleDomSmForrester::vleDomSmForrester(QDomDocument* doc): DomObject(doc)

{

}

vleDomSmForrester::~vleDomSmForrester()
{

}

QString
vleDomSmForrester::getXQuery(QDomNode node)
{
    QString name = node.nodeName();
    if ((name == "definition") or
        (name == "configuration") or
        (name == "constructor") or
        (name == "usersection") or
        (name == "includes") or
        (name == "compartments") or
        (name == "parameters") or
        (name == "flows") or
        (name == "srcPlugin") or
        (name == "dynamic") or
        (name == "observable") or
        (name == "condition") or
        (name == "in") or
        (name == "out")) {
        return getXQuery(node.parentNode())+"/"+name;
    }
    //element identified by attribute name
    if ((name == "compartment") or
        (name == "parameter") or
        (name == "flow")){
        return getXQuery(node.parentNode())+"/"+name+"[@name=\""
                +DomFunctions::attributeValue(node,"name")+"\"]";
    }
    if (node.nodeName() == "vle_project_metadata") {
        return "./vle_project_metadata";
    }
    return "";
}

QDomNode
vleDomSmForrester::getNodeFromXQuery(const QString& query,
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
        (curr == "compartments") or
        (curr == "parameters") or
        (curr == "flows") or
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
    nodeByNames.push_back(QString("compartment"));
    nodeByNames.push_back(QString("parameter"));
    nodeByNames.push_back(QString("flow"));
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

vleSmForrester::vleSmForrester(const QString& srcpath, const QString& smpath,
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

    mVdoSm = new vleDomSmForrester(mDocSm);
    undoStackSm = new DomDiffStack(mVdoSm);

    undoStackSm->init(*mDocSm);
    setCurrentTab("Forrester Diagram");//the first one to open

    QObject::connect(undoStackSm,
                SIGNAL(undoRedoVdo(QDomNode, QDomNode)),
                this, SLOT(onUndoRedoSm(QDomNode, QDomNode)));
    QObject::connect(undoStackSm,
                SIGNAL(undoAvailable(bool)),
                this, SLOT(onUndoAvailable(bool)));
}

void
vleSmForrester::xCreateDom()
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
        xElem = mDocSm->createElement("compartments");
        xDefModel.appendChild(xElem);
        xElem = mDocSm->createElement("parameters");
        xDefModel.appendChild(xElem);
        xElem = mDocSm->createElement("flows");
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


        //set default conditions
        setNumericalIntegration("euler", false);
    }
}

void
vleSmForrester::xReadDom()
{
    if (mDocSm) {
        QFile file(mFileNameSm);
        mDocSm->setContent(&file);
    }
}

void
vleSmForrester::addCompartmentToDoc(const QString& compName, QPointF pos)
{
    if (existCompToDoc(compName)) {
        return;
    }

    QDomNode rootNode = mDocSm->documentElement();
    QDomNode compsNode = mDocSm->elementsByTagName("compartments").item(0);


    undoStackSm->snapshot(rootNode);

    //add compartment
    QDomElement el = mDocSm->createElement("compartment");
    el.setAttribute("name", compName);
    el.setAttribute("x", pos.x());
    el.setAttribute("y", pos.y());
    compsNode.appendChild(el);

    //add observable
    vleDomStatic::addObservablePort(*mDocSm, nodeObs(), compName);

    //add atomic output port
    setType(compName, "Out", false);

    //add condition initial value
    setInitialValue(compName, 0.0, false);
    if (numericalIntegration() == "qss2") {
        setNumericalQuantum(compName, 0.1, false);
    }

    emit modified(OTHER);
}

void
vleSmForrester::addParameterToDoc(const QString& paramName, QPointF pos)
{
    if (existParameterToDoc(paramName)) {
        return;
    }
    QDomNode rootNode = mDocSm->documentElement();
    QDomNode paramsNode = nodeParameters();

    undoStackSm->snapshot(rootNode);

    //add parameter
    QDomElement el = mDocSm->createElement("parameter");
    el.setAttribute("name", paramName);
    el.setAttribute("x", pos.x());
    el.setAttribute("y", pos.y());
    paramsNode.appendChild(el);


    //add condition initial value
    setInitialValue(paramName, 0.0, false);

    emit modified(OTHER);
}

void
vleSmForrester::addMaterialFlowToDoc(const QString& matName, QPointF pos)
{
    if (existMaterialFlowToDoc(matName)) {
        return;
    }

    QDomNode rootNode = mDocSm->documentElement();
    QDomNode matFlowNode = nodeMaterialFlows();


    undoStackSm->snapshot(rootNode);

    QDomElement el = mDocSm->createElement("flow");
    el.setAttribute("name", matName);
    el.setAttribute("x", pos.x());
    el.setAttribute("y", pos.y());
    QDomElement compute = mDocSm->createElement("compute");
    compute.setAttribute("edit", "assist");
    QDomCDATASection cdataCompute = mDocSm->createCDATASection("0");
    compute.appendChild(cdataCompute);
    el.appendChild(compute);
    matFlowNode.appendChild(el);
    vleDomStatic::addObservablePort(*mDocSm, nodeObs(), matName);

    emit modified(OTHER);
}

void
vleSmForrester::setPositionToCompartment(const QString& compName, QPointF pos,
        bool snap)
{
    if (not existCompToDoc(compName)) {
        return;
    }

    QDomNode comp = nodeComp(compName);
    if (snap) {
        vle::value::Map* forUndoMerge = new vle::value::Map();
        forUndoMerge->addString("query", mVdoSm->getXQuery(comp).toStdString());
        undoStackSm->snapshot(comp, "setPositionToCompartment",
                forUndoMerge);
    }
    DomFunctions::setAttributeValue(comp, "x", QVariant(pos.x()).toString());
    DomFunctions::setAttributeValue(comp, "y", QVariant(pos.y()).toString());
    if (snap) {
        emit modified(MOVE_OBJ);
    }
}

void
vleSmForrester::setPositionToParam(const QString& paramName, QPointF pos,
        bool snap)
{
    if (not existParameterToDoc(paramName)) {
        return;
    }

    QDomNode param = nodeParameter(paramName);
    if (snap) {
        vle::value::Map* forUndoMerge = new vle::value::Map();
        forUndoMerge->addString("query", mVdoSm->getXQuery(param).toStdString());
        undoStackSm->snapshot(param, "setPositionToParam",
                forUndoMerge);
    }
    DomFunctions::setAttributeValue(param, "x", QVariant(pos.x()).toString());
    DomFunctions::setAttributeValue(param, "y", QVariant(pos.y()).toString());
    if (snap) {
        emit modified(MOVE_OBJ);
    }
}


void
vleSmForrester::setPositionToMaterialFlow(const QString& matName, QPointF pos,
        bool snap)
{
    if (not existMaterialFlowToDoc(matName)) {
        return;
    }

    QDomNode mat = nodeMaterialFlow(matName);
    if (snap) {
        vle::value::Map* forUndoMerge = new vle::value::Map();
        forUndoMerge->addString("query", mVdoSm->getXQuery(mat).toStdString());
        undoStackSm->snapshot(mat, "setPositionToMaterialFlow",
                forUndoMerge);
    }
    DomFunctions::setAttributeValue(mat, "x", QVariant(pos.x()).toString());
    DomFunctions::setAttributeValue(mat, "y", QVariant(pos.y()).toString());
    if (snap) {
        emit modified(MOVE_OBJ);
    }
}

void
vleSmForrester::setPositionToSoftLink(const QString& matName,
        const QString& compName, QPointF pos, bool snap)
{
    if (not existMaterialFlowToDoc(matName)) {
        return;
    }
    QDomNode mat = nodeMaterialFlow(matName);
    if (snap) {
        vle::value::Map* forUndoMerge = new vle::value::Map();
        forUndoMerge->addString("query", mVdoSm->getXQuery(mat).toStdString());
        undoStackSm->snapshot(mat, "setPositionToSoftLink",
                forUndoMerge);
    }
    QDomNode depends_on = DomFunctions::childWhithNameAttr(mat, "depends_on",
            compName);
    DomFunctions::setAttributeValue(depends_on, "x",
            QVariant(pos.x()).toString());
    DomFunctions::setAttributeValue(depends_on, "y",
            QVariant(pos.y()).toString());
    if (snap) {
        emit modified(MOVE_OBJ);
    }
}

void
vleSmForrester::setAdvancedEdition(const QString& matFlow)
{
    QDomNode matFlowNode = nodeMaterialFlow(matFlow);
    if (matFlowNode.isNull()){
        return ;
    }
    undoStackSm->snapshot(matFlowNode);

    QDomNode computeNode = DomFunctions::obtainChild(
                matFlowNode, "compute", mDocSm);
    QString curr_comp = getCompute(matFlow);
    curr_comp = matFlow+" ="+ curr_comp +";";
    computeNode.toElement().setAttribute("edit", "advanced");

    QDomNodeList childs = computeNode.childNodes();
    if (childs.length() == 0) {
        QDomCDATASection cdataCompute = mDocSm->createCDATASection(curr_comp);
        computeNode.appendChild(cdataCompute);
    } else {
        QDomCDATASection cdataCompute = childs.at(0).toCDATASection();
        cdataCompute.setData(curr_comp);
    }

    emit modified(ADVANCED_EDTION);
}

bool
vleSmForrester::hasAdvancedEdition(const QString& matFlow)
{
    QDomNode matFlowNode = nodeMaterialFlow(matFlow);
    if (matFlowNode.isNull()){
        return false;
    }
    QDomNode computeNode = DomFunctions::obtainChild(
                    matFlowNode, "compute", mDocSm);
    return DomFunctions::attributeValue(computeNode, "edit") == "advanced";
}

void
vleSmForrester::setCompute(const QString& matFlow, const QString& computeBody)
{
    QDomElement docElem = mDocSm->documentElement();
    QDomNode matFlowNode = nodeMaterialFlow(matFlow);

    undoStackSm->snapshot(matFlowNode);

    QDomNode computeNode = DomFunctions::obtainChild(
            matFlowNode, "compute", mDocSm);

    QDomNodeList childs = computeNode.childNodes();
    if (childs.length() == 0) {
        QDomCDATASection cdataCompute = mDocSm->createCDATASection(computeBody);
        computeNode.appendChild(cdataCompute);
    } else {
        QDomCDATASection cdataCompute = childs.at(0).toCDATASection();
        cdataCompute.setData(computeBody);
    }

    emit modified(OTHER);
}

QString
vleSmForrester::getCompute(const QString& matFlow)
{
    QDomElement docElem = mDocSm->documentElement();

    QDomNode matFlowNode = nodeMaterialFlow(matFlow);
    if (matFlowNode.isNull()) {
        return "";
    }
    QDomNode computeNode = DomFunctions::obtainChild(
            matFlowNode, "compute", mDocSm);

    QDomNodeList childs = computeNode.childNodes();
    if (childs.length() == 0) {
        return "";
    } else {
        QDomCDATASection cdataCompute = childs.at(0).toCDATASection();
        return cdataCompute.data();
    }
}

bool
vleSmForrester::usefullForFlow(QString matFlowName, QString otherName)
{
    QDomNode mat = nodeMaterialFlow(matFlowName);
    QDomNode d = DomFunctions::childWhithNameAttr(mat,"depends_on",otherName);
    return not d.isNull();
}

void
vleSmForrester::setUsefullForFlow(QString matFlowName, QString otherName,
        bool val, QPointF centerPoint, bool snapshot)
{
    QDomNode mat = nodeMaterialFlow(matFlowName);
    QDomNode d = DomFunctions::childWhithNameAttr(mat,"depends_on",otherName);
    if (d.isNull() != val) {
        return;
    }
    if (snapshot) {
        undoStackSm->snapshot(mat);
    }
    if (val) {
        QDomNode dep = DomFunctions::childWhithNameAttr(mat,"depends_on",
                otherName, mDocSm);
        double xMat = QVariant(
                DomFunctions::attributeValue(mat, "x")).toDouble();
        double yMat = QVariant(
                DomFunctions::attributeValue(mat, "y")).toDouble();
        DomFunctions::setAttributeValue(dep, "x",
                QVariant((centerPoint.x()+xMat)/2.0).toString());
        DomFunctions::setAttributeValue(dep, "y",
                QVariant((centerPoint.y()+yMat)/2.0).toString());
    } else {
        mat.removeChild(d);
    }
    if (snapshot) {
        emit modified(DEPENDENCY);
    }
}

void
vleSmForrester::setHardLinkTo(QString matFlowName, QString compName, bool val,
        bool snapshot)
{
    if (not val and hardLinkTo(matFlowName) != compName) {
        return;
    }

    QDomNode mat = nodeMaterialFlow(matFlowName);
    if (snapshot) {
        undoStackSm->snapshot(mat);
    }
    QDomNode hard_link_to = DomFunctions::obtainChild(mat, "hard_link_to");
    if (val) {
        setHardLinkFrom(matFlowName, compName, false, false);
        if (hard_link_to.isNull()) {
            hard_link_to = DomFunctions::childWhithNameAttr(mat,"hard_link_to",
                            compName, mDocSm);
        } else {
            DomFunctions::setAttributeValue(hard_link_to,"name", compName);
        }

    } else {
        if (not hard_link_to.isNull()) {
            mat.removeChild(hard_link_to);
        }
        setUsefullForFlow(matFlowName, compName, false, QPointF(), false);
    }
    if (snapshot) {
        emit modified(DEPENDENCY);
    }
}

void
vleSmForrester::setHardLinkFrom(QString matFlowName, QString compName, bool val,
        bool snapshot)
{
    if (not val and hardLinkFrom(matFlowName) != compName) {
        return;
    }

    QDomNode mat = nodeMaterialFlow(matFlowName);
    if (snapshot) {
        undoStackSm->snapshot(mat);
    }
    QDomNode hard_link_from = DomFunctions::obtainChild(mat, "hard_link_from");
    if (val) {
        setHardLinkTo(matFlowName, compName, false, false);
        if (hard_link_from.isNull()) {
            hard_link_from = DomFunctions::childWhithNameAttr(mat,
                    "hard_link_from", compName, mDocSm);
        } else {
            DomFunctions::setAttributeValue(hard_link_from,"name", compName);
        }
    } else {
        if (not hard_link_from.isNull()) {
            mat.removeChild(hard_link_from);
        }
        setUsefullForFlow(matFlowName, compName, false, QPointF(), false);
    }
    if (snapshot) {
        emit modified(DEPENDENCY);
    }
}

QString
vleSmForrester::hardLinkTo(QString matFlowName) const
{
    QDomNode mat = nodeMaterialFlow(matFlowName);
    QList<QDomNode> d = DomFunctions::childNodesWithoutText(
            mat,"hard_link_to");
    if (d.size() != 1) {
        return "";
    }
    return (DomFunctions::attributeValue(d.at(0), "name"));
}

QString
vleSmForrester::hardLinkFrom(QString matFlowName) const
{
    QDomNode mat = nodeMaterialFlow(matFlowName);
    QList<QDomNode> d = DomFunctions::childNodesWithoutText(
            mat,"hard_link_from");
    if (d.size() != 1) {
        return "";
    }
    return (DomFunctions::attributeValue(d.at(0), "name"));
}

QSet<QString>
vleSmForrester::linksToComp(QString compName) const
{
    QSet<QString> links_to_comp;
    QDomNodeList mats = materialFlowsFromDoc();
    for (int i=0; i<mats.size();i++) {
        QDomNode mat=mats.at(i);
        if(not DomFunctions::childWhithNameAttr(mat,
                "hard_link_to", compName).isNull()) {
            links_to_comp.insert(DomFunctions::attributeValue(mat, "name"));
        }
    }
    return links_to_comp;
}

QSet<QString>
vleSmForrester::linksFromComp(QString compName) const
{
    QSet<QString> links_from_comp;
    QDomNodeList mats = materialFlowsFromDoc();
    for (int i=0; i<mats.size();i++) {
        QDomNode mat=mats.at(i);
        if(not DomFunctions::childWhithNameAttr(mat,
                "hard_link_from", compName).isNull()) {
            links_from_comp.insert(DomFunctions::attributeValue(mat, "name"));
        }
    }
    return links_from_comp;
}

/**
 * @brief get the numerical integration method
 */
QString
vleSmForrester::numericalIntegration() const
{

    QDomNode portMethod = nodeCondPort("method");
    QDomNode nodeMethod = portMethod.firstChildElement();
    if (nodeMethod.isNull()) {
        return "";
    }
    std::unique_ptr<vle::value::Value> method =
            vleDomStatic::buildValue(nodeMethod, false);

    return QString(method->toString().value().c_str());
}

double
vleSmForrester::numericalTimeStep() const
{
    QDomNode portTS = nodeCondPort("time_step");
    if (portTS.isNull()) {
        return 0;
    }
    QDomNode nodeTS = portTS.firstChildElement();
    std::unique_ptr<vle::value::Value> ts =
            vleDomStatic::buildValue(nodeTS, false);
    return ts->toDouble().value();
}

double
vleSmForrester::numericalQuantum(QString compName) const
{
    QDomNode portQT = nodeCondPort("quantum_"+compName);
    if (portQT.isNull()) {
        return 0;
    }
    QDomNode nodeQT = portQT.firstChildElement();
    std::unique_ptr<vle::value::Value> qt =
                vleDomStatic::buildValue(nodeQT, false);
    return qt->toDouble().value();
}

void
vleSmForrester::setNumericalIntegration(QString method, bool snap)
{
    QDomNode cond = nodeCond();
    if (snap) {
        undoStackSm->snapshot(cond);
    }
    vle::value::Map tofill;

    QString currInt = numericalIntegration();
    //remove time_step and add quantum if required
    if ((currInt == "euler" or currInt == "rk4")
            and method == "qss2") {
        vleDomStatic::rmPortFromCond(cond, "time_step");
        for (auto comp : compartmentNames()) {

            std::string quantum_i = "quantum_";
            quantum_i += comp.toStdString();
            tofill.add(quantum_i, vle::value::Double::create(0.1));
        }

    }
    //remove quantum and add time step if required
    if ((method == "euler" or method == "rk4")
            and currInt == "qss2") {
        for (auto comp : compartmentNames()) {
            vleDomStatic::rmPortFromCond(cond, "quantum_"+comp);
        }
        tofill.add("time_step", vle::value::Double::create(0.1));
    }
    //in case numerical integration has not be defined before
    if ((method == "euler" or method == "rk4")
            and currInt == "") {
        tofill.add("time_step", vle::value::Double::create(0.1));
    }
    //set integration
    tofill.add("method", vle::value::String::create(method.toStdString()));

    vleDomStatic::fillConditionWithMap(*mDocSm, cond, tofill);

    if (snap) {
        emit modified(NUMERICAL_INT);
    }
}

void
vleSmForrester::setNumericalTimeStep(double ts, bool snap)
{
    QDomNode cond = nodeCond();
    if (snap) {
        undoStackSm->snapshot(cond);
    }
    vle::value::Map tofill;
    tofill.add("time_step", vle::value::Double::create(ts));
    vleDomStatic::fillConditionWithMap(*mDocSm, cond, tofill);
    if (snap) {
        emit modified(NUMERICAL_INT);
    }
}

void
vleSmForrester::setNumericalQuantum(QString compName, double qt, bool snap)
{
    QDomNode cond = nodeCond();
    if (snap) {
        undoStackSm->snapshot(cond);
    }
    vle::value::Map tofill;
    std::string quantum_i = "quantum_";
    quantum_i += compName.toStdString();
    tofill.add(quantum_i, vle::value::Double::create(qt));
    vleDomStatic::fillConditionWithMap(*mDocSm, cond, tofill);
    if (snap) {
        emit modified(NUMERICAL_INT);
    }
}


void
vleSmForrester::setType(const QString& compName, const QString& type, bool snap)
{
    QDomNode inNode = nodeIn();
    QDomNode outNode = nodeOut();
    QDomNode configNode = nodeConfiguration();
    if (snap) {
        undoStackSm->snapshot(configNode);
    }
    if (type == "In") {
        vleDomStatic::rmPortToOutNode(outNode, compName, 0);
        vleDomStatic::addPortToInNode(*mDocSm, inNode, compName, 0);
    } else if (type == "Out") {
        vleDomStatic::rmPortToInNode(inNode, compName, 0);
        vleDomStatic::addPortToOutNode(*mDocSm, outNode, compName, 0);
    } else if (type == "In/Out") {
        vleDomStatic::addPortToInNode(*mDocSm, inNode, compName, 0);
        vleDomStatic::addPortToOutNode(*mDocSm, outNode, compName, 0);
    }
    if (snap) {
        emit modified(OTHER);
    }

}


void
vleSmForrester::setInitialValue(const QString& varName,
                         double val, bool snap)
{
    if (nodeVar(varName).isNull()) {
        return;
    }

    QDomNode cond = nodeCond();

    if (snap) {
        undoStackSm->snapshot(cond);
    }
    vle::value::Map initPort;
    initPort.add("init_value_"+varName.toStdString(),
            vle::value::Double::create(val));
    vleDomStatic::fillConditionWithMap(*mDocSm, cond, initPort);

    if (snap)  {
        emit modified(OTHER);
    }
}

std::unique_ptr<vv::Value>
vleSmForrester::getInitialValue(const QString& varName) const
{
    if (nodeVar(varName).isNull()) {
        return nullptr;
    }

    QDomNode cond = nodeCond();
    QString init_name = "init_value_" + varName;
    QDomNode port = nodeCondPort("init_value_" + varName);
    if (port.isNull()) {
        return std::unique_ptr<vv::Value>();
    }
    QDomNode init_value = port.firstChildElement();
    return std::move(vleDomStatic::buildValue(init_value, false));
}

void
vleSmForrester::unsetInitialValue(const QString& varName, bool snap)
{
    if (nodeVar(varName).isNull()) {
        return;
    }
    QDomNode cond = nodeCond();
    if (snap) {
        undoStackSm->snapshot(cond);
    }

    QString portName = "init_value_"+varName;
    vleDomStatic::rmPortFromCond(cond, portName);
    if (snap) {
        emit modified(OTHER);
    }
}

void
vleSmForrester::renameCompartmentToDoc(const QString& oldName,
        const QString& newName)
{
    QDomNode rootNode = mDocSm->documentElement();

    QDomNode comp = nodeComp(oldName);
    if (comp.isNull()) {
        return ;
    }

    undoStackSm->snapshot(rootNode);
    //rename compartment
    DomFunctions::setAttributeValue(comp, "name", newName);
    //rename condition port for initial value
    vleDomStatic::renamePortFromCond(nodeCond(), "init_value_"+oldName,
            "init_value_"+newName, 0);
    //rename observable port
    vleDomStatic::renameObservablePort(nodeObs(), oldName, newName, 0);
    vleDomStatic::renamePortToOutNode(nodeOut(), oldName,
            newName, 0);
    vleDomStatic::renamePortToInNode(nodeIn(), oldName,
            newName, 0);

    QDomNodeList matFlows = materialFlowsFromDoc();
    for (int i=0; i< matFlows.length(); i++) {
        QDomNode flow = matFlows.at(i);
        QDomNode dep = DomFunctions::childWhithNameAttr(flow,
                        "depends_on", oldName);
        if (not dep.isNull()) {
            DomFunctions::setAttributeValue(dep, "name", newName);
        }
    }

    emit modified(RENAME);
}

void
vleSmForrester::renameParameterToDoc(const QString& oldName,
        const QString& newName)
{
    QDomNode param = nodeParameter(oldName);
    if (param.isNull()) {
        return ;
    }
    undoStackSm->snapshot(param);
    DomFunctions::setAttributeValue(param, "name", newName);
    //rename condition port for parameter value
    vleDomStatic::renamePortFromCond(nodeCond(), "init_value_"+oldName,
            "init_value_"+newName, 0);
    QDomNodeList matFlows = materialFlowsFromDoc();
    for (int i=0; i< matFlows.length(); i++) {
        QDomNode flow = matFlows.at(i);
        QDomNode dep = DomFunctions::childWhithNameAttr(flow,
                "depends_on", oldName);
        if (not dep.isNull()) {
            DomFunctions::setAttributeValue(dep, "name", newName);
        }
    }
    emit modified(RENAME);
}

void
vleSmForrester::renameMaterialFlowToDoc(const QString& oldName,
        const QString& newName)
{
    QDomNode matFlow = nodeMaterialFlow(oldName);
    if (matFlow.isNull()) {
        return ;
    }
    undoStackSm->snapshot(matFlow);
    DomFunctions::setAttributeValue(matFlow, "name", newName);
    //rename observable port
    vleDomStatic::renameObservablePort(nodeObs(), oldName, newName, 0);

    emit modified(RENAME);
}

QString
vleSmForrester::newCompartmentNameToDoc()
{
    return DomFunctions::childNameProvider(nodeComps(),"compartment",
            "compartment");
}

QString
vleSmForrester::newParameterNameToDoc()
{
    return DomFunctions::childNameProvider(nodeParameters(),"parameter",
            "parameter");
}

QString
vleSmForrester::newMaterialFlowNameToDoc()
{
    return DomFunctions::childNameProvider(nodeMaterialFlows(),"flow",
            "flow");
}

bool
vleSmForrester::existCompToDoc(QString compName)
{
    QDomNode comp = nodeComp(compName);
    return not comp.isNull();
}

bool
vleSmForrester::existParameterToDoc(QString paramName)
{
    QDomNode mat = nodeParameter(paramName);
    return not mat.isNull();
}

bool
vleSmForrester::existMaterialFlowToDoc(QString matName)
{
    QDomNode mat = nodeMaterialFlow(matName);
    return not mat.isNull();
}

void
vleSmForrester::rmCompartmentToDoc(const QString& compName)
{
    if (not existCompToDoc(compName)) {
        return;
    }
    undoStackSm->snapshot(mDocSm->documentElement());
    unsetInitialValue(compName, false);

    QDomNode compsNode = nodeComps();
    QDomNode compNode = nodeComp(compName);
    compsNode.removeChild(compNode);

    vleDomStatic::rmPortToInNode(nodeIn(),compName);
    vleDomStatic::rmPortToOutNode(nodeOut(),compName);
    vleDomStatic::rmObservablePort(nodeObs(), compName);



    emit modified(RENAME);
}
void
vleSmForrester::rmMateriaFlowToDoc(const QString& matName)
{
    if (not existMaterialFlowToDoc(matName)) {
        return;
    }
    undoStackSm->snapshot(mDocSm->documentElement());

    QDomNode matsNode = nodeMaterialFlows();
    QDomNode matNode = nodeMaterialFlow(matName);
    matsNode.removeChild(matNode);

    vleDomStatic::rmObservablePort(nodeObs(), matName);

    emit modified(RENAME);
}

void
vleSmForrester::rmParameterToDoc(const QString& paramName)
{
    if (not existParameterToDoc(paramName)) {
        return;
    }
    undoStackSm->snapshot(mDocSm->documentElement());

    QDomNode paramssNode = nodeParameters();
    QDomNode paramNode = nodeParameter(paramName);
    paramssNode.removeChild(paramNode);

    emit modified(RENAME);
}

void
vleSmForrester::setConstructorToDoc(const QString& constructorBody)
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

    emit modified(OTHER);
}

QString
vleSmForrester::getConstructorBody()
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
vleSmForrester::setUserSectionToDoc(const QString& usersectionBody)
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

    emit modified(OTHER);
}

QString
vleSmForrester::getUserSectionBody()
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
vleSmForrester::setIncludesToDoc(const QString& includesBody)
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

    emit modified(OTHER);
}

QString
vleSmForrester::getIncludesBody()
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
vleSmForrester::getSrcPlugin()
{
    QDomElement docElem = mDocSm->documentElement();

    QDomNode srcPluginNode =
        mDocSm->elementsByTagName("srcPlugin").item(0);
    return srcPluginNode.attributes().namedItem("name").nodeValue();
}

void
vleSmForrester::setClassNameToDoc(const QString& className, bool snap)
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
        emit modified(OTHER);
    }
}

QString
vleSmForrester::getClassName()
{
    QDomElement docElem = mDocSm->documentElement();

    QDomNode srcPluginNode =
        mDocSm->elementsByTagName("srcPlugin").item(0);
    return srcPluginNode.attributes().namedItem("class").nodeValue();
}

void
vleSmForrester::setPackageToDoc(const QString& nm, bool snap)
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
        emit modified(OTHER);
    }
}

QString
vleSmForrester::getPackage()
{
    QDomElement docElem = mDocSm->documentElement();

    QDomNode srcPluginNode =
        mDocSm->elementsByTagName("srcPlugin").item(0);
    return srcPluginNode.attributes().namedItem("package").nodeValue();
}

QDomNodeList
vleSmForrester::compartmentsFromDoc()
{
    return nodeComps().toElement().elementsByTagName("compartment");
}

QDomNodeList
vleSmForrester::parametersFromDoc()
{
    return nodeParameters().toElement().elementsByTagName("parameter");
}

QDomNodeList
vleSmForrester::materialFlowsFromDoc() const
{
    return nodeMaterialFlows().toElement().elementsByTagName("flow");
}

QSet<QString>
vleSmForrester::compartmentNames()
{
    return DomFunctions::childNames(nodeComps(), "compartment");
}

QSet<QString>
vleSmForrester::parameterNames()
{
    return DomFunctions::childNames(nodeParameters(), "parameter");
}

QSet<QString>
vleSmForrester::materialFlowNames()
{
    return DomFunctions::childNames(nodeMaterialFlows(), "flow");
}

void
vleSmForrester::setCurrentTab(QString tabName)
{
    undoStackSm->current_source = tabName;
}

void
vleSmForrester::save()
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
vleSmForrester::provideCpp()
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
vleSmForrester::undo()
{
    waitUndoRedoSm = true;
    undoStackSm->undo();
    undoStackSm->print(std::cout);//dbg
    emit modified(OTHER);
}

void
vleSmForrester::redo()
{
    waitUndoRedoSm = true;
    undoStackSm->redo();
    emit modified(OTHER);
}

QString
vleSmForrester::getData()
{
    QString tpl = "/**\n"                                               \
        "  * @file {{classname}}.cpp\n"                                 \
        "  * @author ...\n"                                             \
        "  * ...\n"                                                     \
        "  */\n\n"                                                      \
        "/*\n"                                                          \
        " * @@tagdynamic@@\n"                                           \
        " * @@tagdepends: vle.ode @@endtagdepends\n"                    \
        "*/\n\n"                                                        \
        "#include <vle/DifferentialEquation.hpp>\n"                     \
        "{{includes}}\n"                                                \
        "namespace vd = vle::devs;\n\n"                                 \
        "namespace vv = vle::value;\n\n"                                \
        "namespace vle {\n"                                             \
        "namespace ode {\n"                                             \
        "namespace {{namespace}} {\n\n"                                 \
        "using namespace vle::ode;\n"                                   \
        "class {{classname}} : public DifferentialEquation\n"           \
        "{\n"                                                           \
        "public:\n"                                                     \
        "{{classname}}(\n"                                              \
        "    const vd::DynamicsInit& init,\n"                           \
        "    const vd::InitEventList& evts)\n"                          \
        "        : DifferentialEquation(init, evts)\n"                  \
        "{\n"                                                           \
        "{{for i in obj}}"                                              \
        "    {{obj^i}}.init(this, \"{{obj^i}}\", evts);\n"              \
        "{{end for}}"                                                   \
        "{{construct}}\n"                                               \
        "\n"                                                            \
        "}\n"                                                           \
        "\n"                                                            \
        "virtual ~{{classname}}()\n"                                    \
        "{}\n"                                                          \
        "\n"                                                            \
        "void compute(const vle::devs::Time& t) override\n"             \
        "{\n"                                                           \
        "{{compute_str}}\n"                                             \
        "}\n"                                                           \
        "\n"                                                            \
        "{{for i in obj}}"                                              \
        "    Var {{obj^i}};\n"                                          \
        "{{end for}}"                                                   \
        "{{userSection}}\n"                                             \
        "};\n\n"                                                        \
        "} // namespace {{namespace}}\n"                                \
        "} // namespace ode\n"                                          \
        "} // namespace vle\n\n"                                        \
        "DECLARE_DYNAMICS("                                             \
        "vle::ode::{{namespace}}::{{classname}})\n\n";

    vle::utils::Template vleTpl(tpl.toStdString());
    vleTpl.stringSymbol().append("classname", getClassName().toStdString());
    vleTpl.stringSymbol().append("namespace",
            getPackage().replace('.', '_').toStdString());
    vleTpl.listSymbol().append("obj");

    QSet<QString> matsNames = materialFlowNames();
    QSet<QString> paramsNames = parameterNames();
    QDomNodeList compsXml = compartmentsFromDoc();

    std::string compute_str;

    for (auto mat : matsNames) {
        vleTpl.listSymbol().append("obj", mat.toStdString());
        if (hasAdvancedEdition(mat)) {
            compute_str += "\n"+getCompute(mat).toStdString();
        } else {
            compute_str += "\n"+mat.toStdString()+ " = "
                                +getCompute(mat).toStdString()+";";
        }
    }
    compute_str += "\n";


    for (int i = 0; i < compsXml.length(); i++) {
        QDomNode comp = compsXml.item(i);
        QString compName = comp.attributes().namedItem("name").nodeValue();
        vleTpl.listSymbol().append("obj", compName.toStdString());
        compute_str += "\n grad("+compName.toStdString()+") = 0";
        for (auto flow : linksToComp(compName)) {
            compute_str += "+ "+flow.toStdString()+"()";
        }
        for (auto flow : linksFromComp(compName)) {
            compute_str += "- "+flow.toStdString()+"()";
        }
        compute_str += ";";
    }

    for (auto param : paramsNames) {
        vleTpl.listSymbol().append("obj", param.toStdString());
    }
    compute_str += "\n";

    vleTpl.stringSymbol().append("compute_str", compute_str);
    vleTpl.stringSymbol().append("includes", getIncludesBody().toStdString());
    vleTpl.stringSymbol().append("construct", getConstructorBody().toStdString());
    vleTpl.stringSymbol().append("userSection", getUserSectionBody().toStdString());

    std::ostringstream out;
    vleTpl.process(out);

    return QString(out.str().c_str());
}


QDomNode
vleSmForrester::nodeComps() const
{
    return mDocSm->elementsByTagName("compartments").item(0);
}

QDomNode
vleSmForrester::nodeParameters() const
{
    return mDocSm->elementsByTagName("parameters").item(0);
}

QDomNode
vleSmForrester::nodeMaterialFlows() const
{
    return mDocSm->elementsByTagName("flows").item(0);
}

QDomNode
vleSmForrester::nodeComp(const QString& compName) const
{
    QDomNode vars = nodeComps();
    return DomFunctions::childWhithNameAttr(vars , "compartment", compName);
}

QDomNode
vleSmForrester::nodeParameter(const QString& paramName) const
{
    QDomNode vars = nodeParameters();
    return DomFunctions::childWhithNameAttr(vars , "parameter", paramName);
}

QDomNode
vleSmForrester::nodeVar(const QString& varName) const
{
    QDomNode input = nodeComp(varName);
    if (not input.isNull()) {
        return input;
    }
    return nodeParameter(varName);
}



QDomNode
vleSmForrester::nodeMaterialFlow(const QString& matName) const
{
    QDomNode vars = nodeMaterialFlows();
    return DomFunctions::childWhithNameAttr(vars , "flow", matName);
}


QDomNode
vleSmForrester::nodeCond() const
{
    QDomNode conditionNode = mDocSm->elementsByTagName("condition").item(0);
    return conditionNode;
}

QDomNode
vleSmForrester::nodeCondPort(const QString& portName) const
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
vleSmForrester::nodeObs() const
{
    QDomNode conditionNode = mDocSm->elementsByTagName("observable").item(0);
    return conditionNode;
}

QDomNode
vleSmForrester::nodeObsPort(const QString& portName)
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
vleSmForrester::nodeIn() const
{
    QDomNode inNode = mDocSm->elementsByTagName("in").item(0);
    return inNode;
}

QDomNode
vleSmForrester::nodeOut() const
{
    QDomNode outNode = mDocSm->elementsByTagName("out").item(0);
    return outNode;
}

QDomNode
vleSmForrester::nodeConfiguration() const
{
    QDomNode outNode = mDocSm->elementsByTagName("configuration").item(0);
    return outNode;
}

QDomNode
vleSmForrester::nodeInPort(const QString& portName)
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
vleSmForrester::nodeOutPort(const QString& portName)
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
vleSmForrester::onUndoRedoSm(QDomNode oldSm, QDomNode newSm)
{
    oldValSm = oldSm;
    newValSm = newSm;
    waitUndoRedoSm = false;
    emit undoRedo(oldValSm, newValSm);
}

void
vleSmForrester::onUndoAvailable(bool b)
{
    emit undoAvailable(b);
}

}
}
