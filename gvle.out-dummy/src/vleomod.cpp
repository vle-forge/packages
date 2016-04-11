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
#include "vleomod.h"

namespace vle {
namespace gvle {

vleDomOmOD::vleDomOmOD(QDomDocument* doc): vleDomObject(doc)

{

}

vleDomOmOD::~vleDomOmOD()
{

}

QString
vleDomOmOD::getXQuery(QDomNode node)
{
    QString name = node.nodeName();
    if (name == "outPlugin") {
        return getXQuery(node.parentNode())+"/"+name;
    }
    if (node.nodeName() == "vle_project_metadata") {
        return "./vle_project_metadata";
    }
    return "";
}

QDomNode
vleDomOmOD::getNodeFromXQuery(const QString& query,
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
    if ((curr == "outPlugin")){
        return getNodeFromXQuery(rest, obtainChild(d, curr, true));
    }
    return QDomNode();
}

/************************************************************************/

vleOmOD::vleOmOD(const QString& srcpath, const QString& ompath,
        QString pluginName):
    mDocOm(0), mFileNameSrc(srcpath), mFileNameOm(ompath), mVdoOm(0),
    undoStackOm(0), waitUndoRedoOm(false),
    oldValOm(), newValOm(), mpluginName(pluginName)
{
    QFile file(mFileNameOm);
    if (file.exists()) {
        mDocOm = new QDomDocument("vle_project_metadata");
        QXmlInputSource source(&file);
        QXmlSimpleReader reader;
        mDocOm->setContent(&source, &reader);
    } else {
        xCreateDom();
    }

    mVdoOm = new vleDomOmOD(mDocOm);
    undoStackOm = new vleDomDiffStack(mVdoOm);
    undoStackOm->init(*mDocOm);

    QObject::connect(undoStackOm,
                SIGNAL(undoRedoVdo(QDomNode, QDomNode)),
                this, SLOT(onUndoRedoOm(QDomNode, QDomNode)));
    QObject::connect(undoStackOm,
                SIGNAL(undoAvailable(bool)),
                this, SLOT(onUndoAvailable(bool)));
}

QString
vleOmOD::toQString(const QDomNode& node) const
{
    QString str;
    QTextStream stream(&str);
    node.save(stream, node.nodeType());
    return str;
}

void
vleOmOD::xCreateDom()
{
    if (not mDocOm) {
        mDocOm = new QDomDocument("vle_project_metadata");
        QDomProcessingInstruction pi;
        pi = mDocOm->createProcessingInstruction("xml",
                "version=\"1.0\" encoding=\"UTF-8\" ");
        mDocOm->appendChild(pi);

        QDomElement vpmRoot = mDocOm->createElement("vle_project_metadata");
        // Save VPZ file revision
        vpmRoot.setAttribute("version", "1.x");
        // Save the author name (if known)
        vpmRoot.setAttribute("author", "meto");
        QDomElement xCondPlug = mDocOm->createElement("outPlugin");
        xCondPlug.setAttribute("name", mpluginName);
        vpmRoot.appendChild(xCondPlug);
        mDocOm->appendChild(vpmRoot);
    }
}

void
vleOmOD::xReadDom()
{
    if (mDocOm) {
        QFile file(mFileNameOm);
        mDocOm->setContent(&file);
    }
}

QString
vleOmOD::getOutPlugin()
{
    QDomElement docElem = mDocOm->documentElement();

    QDomNode outPluginNode =
        mDocOm->elementsByTagName("outPlugin").item(0);
    return outPluginNode.attributes().namedItem("name").nodeValue();
}

void
vleOmOD::setCurrentTab(QString tabName)
{
    undoStackOm->current_source = tabName;
}

void
vleOmOD::save()
{
    QFile file(mFileNameOm);
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
    QByteArray xml = mDocOm->toByteArray();
    file.write(xml);
    file.close();

    undoStackOm->clear();
}

void
vleOmOD::undo()
{
    waitUndoRedoOm = true;
    undoStackOm->undo();
    emit modified();

}

void
vleOmOD::redo()
{
    waitUndoRedoOm = true;
    undoStackOm->redo();
    emit modified();
}

void
vleOmOD::onUndoRedoOm(QDomNode oldOm, QDomNode newOm)
{
    oldValOm = oldOm;
    newValOm = newOm;
    waitUndoRedoOm = false;
    emit undoRedo(oldValOm, newValOm);
}

void
vleOmOD::onUndoAvailable(bool b)
{
    emit undoAvailable(b);
}


}}//namespaces
