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



#include <QFileInfo>
#include <QDir>
#include <QtXml/QDomNode>

#include <vle/utils/Template.hpp>
#include "vlesmdt.h"

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
    if ((name == "compute") or
        (name == "variables") or
        (name == "srcPlugin")) {
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
    if ((curr == "variables") or
        (curr == "compute") or
        (curr == "srcPlugin")){
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
        // Save VPZ file revision
        vpmRoot.setAttribute("version", "1.x");
        // Save the author name (if known)
        vpmRoot.setAttribute("author", "meto");
        QDomElement xCondPlug = mDocSm->createElement("variables");
        vpmRoot.appendChild(xCondPlug);
        xCondPlug = mDocSm->createElement("compute");
        vpmRoot.appendChild(xCondPlug);
        xCondPlug = mDocSm->createElement("srcPlugin");
        xCondPlug.setAttribute("name", mpluginName);
        vpmRoot.appendChild(xCondPlug);
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
    QDomElement docElem = mDocSm->documentElement();
    QDomNode variablesNode =
            mDocSm->elementsByTagName("variables").item(0);
    if (existVarToDoc(varName)) {
        return;
    }

    undoStackSm->snapshot(variablesNode);

    QDomElement el = mDocSm->createElement("variable");
    el.setAttribute("name", varName);
    variablesNode.appendChild(el);

}

void
vleSmDT::setInitialValue(const QString& varName,
        const vle::value::Value& val)
{
    QDomNode var = nodeVariable(varName);
    if (var.isNull()) {
        return;
    }
    undoStackSm->snapshot(var);
    QDomNodeList initList = var.toElement().elementsByTagName("initial_value");
    if (initList.length() == 0) {
        //TODO see vpz management of vle values
        QDomElement dble = mDocSm->createElement("double");
        dble.appendChild(mDocSm->createTextNode(
                val.writeToString().c_str()));
        QDomElement el = mDocSm->createElement("initial_value");
        el.appendChild(dble);
        var.appendChild(el);
    } else {
        QDomNode init_value = initList.at(0);
        init_value = var.toElement().elementsByTagName("double").at(0);
        if (init_value.nodeName() == "double") {
            QDomText dbleval = init_value.childNodes().item(0).toText();
            dbleval.setData(val.writeToString().c_str());
        }
    }
}

vle::value::Value*
vleSmDT::getInitialValue(const QString& varName)
{
    QStringList res;
    QDomNode var = nodeVariable(varName);
    if (var.isNull()) {
        return 0;
    }
    QDomNodeList initList = var.toElement().elementsByTagName("initial_value");
    if (initList.length() == 0) {
        return 0;
    }

    QDomNode init_value = initList.at(0);
    init_value = var.toElement().elementsByTagName("double").at(0);
    if (init_value.nodeName() == "double") {
        QVariant qv = init_value.childNodes().item(0).toText().nodeValue();;
        return new vle::value::Double(qv.toDouble());
    }
    return 0;
}

void
vleSmDT::rmInitialValue(const QString& varName)
{
    QDomNode var = nodeVariable(varName);
    if (var.isNull()) {
        return;
    }
    QDomNodeList initList = var.toElement().elementsByTagName("initial_value");
    if (initList.length() == 0) {
        return;
    }
    undoStackSm->snapshot(var);
    var.removeChild(initList.at(0));
    return ;
}



void
vleSmDT::rmVariableToDoc(const QString& variableName)
{
    QDomElement docElem = mDocSm->documentElement();

    QDomNode variablesNode =
            mDocSm->elementsByTagName("variables").item(0);

    undoStackSm->snapshot(variablesNode);

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

    emit modified();
}


void
vleSmDT::renameVariableToDoc(const QString& oldVariableName,
        const QString& newVariableName)
{
    QDomElement docElem = mDocSm->documentElement();

    QDomNode variablesNode =
            mDocSm->elementsByTagName("variables").item(0);

    QDomNode var = nodeVariable(oldVariableName);
    if (var.isNull()) {
        return ;
    }

    undoStackSm->snapshot(variablesNode);

    for (int j=0; j< var.attributes().size(); j++) {
        if (var.attributes().item(j).nodeName() == "name")  {
            var.attributes().item(j).setNodeValue(newVariableName);
        }
    }
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
    QDomElement docElem = mDocSm->documentElement();

    QDomNode srcPluginNode =
        mDocSm->elementsByTagName("srcPlugin").item(0);

    undoStackSm->snapshot(srcPluginNode);

    srcPluginNode.toElement().setAttribute("class", className);
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
vleSmDT::setNamespaceToDoc(const QString& nm)
{
    QDomElement docElem = mDocSm->documentElement();

    QDomNode srcPluginNode =
        mDocSm->elementsByTagName("srcPlugin").item(0);

    undoStackSm->snapshot(srcPluginNode);

    srcPluginNode.toElement().setAttribute("namespace", nm);
    emit modified();
}

QString
vleSmDT::getNamespace()
{
    QDomElement docElem = mDocSm->documentElement();

    QDomNode srcPluginNode =
        mDocSm->elementsByTagName("srcPlugin").item(0);
    return srcPluginNode.attributes().namedItem("namespace").nodeValue();
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

    if (mFileNameSrc != "") {
        QFile fileCpp(mFileNameSrc);
        if (fileCpp.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
            fileCpp.write(getData().toStdString().c_str()) ;
            fileCpp.flush();
            fileCpp.close();
        }
    }
    undoStackSm->clear();
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
        "#include <vle/DiscreteTimeDbg.hpp>\n"                          \
        "#include <vle/devs/DynamicsDbg.hpp>\n\n"                       \
        "{{includes}}"                                                  \
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
        "{{for i in init_value_var}}"                                   \
        "    {{init_value_var^i}}.init_value({{init_value_val^i}});\n"  \
        "{{end for}}"                                                   \
        "{{construct}}"                                                 \
        "\n"                                                            \
        "}\n"                                                           \
        "\n"                                                            \
        "virtual ~{{classname}}()\n"                                    \
        "{}\n"                                                          \
        "\n"                                                            \
        "void compute(const vle::devs::Time& t)\n"                      \
        "{\n"                                                           \
        "{{compute}}"                                                   \
        "}\n"                                                           \
        "\n"                                                            \
        "{{for i in var}}"                                              \
        "    Var {{var^i}};\n"                                          \
        "{{end for}}"                                                   \
        "{{userSection}}"                                               \
        "};\n\n"                                                        \
        "} // namespace {{namespace}}\n"                                \
        "} // namespace discrete_time\n"                                \
        "} // namespace vle\n\n"                                        \
        "DECLARE_DYNAMICS("                                             \
        "vle::discrete_time::{{namespace}}::{{classname}})\n\n";


    vle::utils::Template vleTpl(tpl.toStdString());
    vleTpl.stringSymbol().append("classname", getClassName().toStdString());
    vleTpl.stringSymbol().append("namespace", getNamespace().toStdString());

    vleTpl.listSymbol().append("var");
    vleTpl.listSymbol().append("init_value_var");
    vleTpl.listSymbol().append("init_value_val");


    QDomNodeList variablesXml = variablesFromDoc();
    for (int i = 0; i < variablesXml.length(); i++) {
        QDomNode variable = variablesXml.item(i);
        QString varName = variable.attributes().namedItem("name").nodeValue();
        vle::value::Value* val = getInitialValue(varName);
        if (val) {
            vleTpl.listSymbol().append("init_value_var", varName.toStdString());
            vleTpl.listSymbol().append("init_value_val", val->writeToString());
            delete val;
        }

        vleTpl.listSymbol().append("var", varName.toStdString());


    }

    vleTpl.stringSymbol().append("includes", "");
    vleTpl.stringSymbol().append("compute", getComputeBody().toStdString());
    vleTpl.stringSymbol().append("construct","");
    vleTpl.stringSymbol().append("userSection","");

    std::ostringstream out;
    vleTpl.process(out);

    return QString(out.str().c_str());
}


QDomNode
vleSmDT::nodeVariable(const QString& varName)
{
    QDomElement docElem = mDocSm->documentElement();
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


}}//namespaces
