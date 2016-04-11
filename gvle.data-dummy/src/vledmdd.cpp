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
#include "vledmdd.h"

namespace vle {
namespace gvle {

vleDomDmDD::vleDomDmDD(QDomDocument* doc): vleDomObject(doc)

{

}

vleDomDmDD::~vleDomDmDD()
{

}

QString
vleDomDmDD::getXQuery(QDomNode node)
{
    QString name = node.nodeName();
    if (name == "dataPlugin") {
        return getXQuery(node.parentNode())+"/"+name;
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
    if ((curr == "dataPlugin")){
        return getNodeFromXQuery(rest, obtainChild(d, curr, true));
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
    undoStackDm = new vleDomDiffStack(mVdoDm);
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
        QDomElement xCondPlug = mDocDm->createElement("dataPlugin");
        xCondPlug.setAttribute("name", mpluginName);
        vpmRoot.appendChild(xCondPlug);
        mDocDm->appendChild(vpmRoot);
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

QString
vleDmDD::getDataPlugin()
{
    QDomElement docElem = mDocDm->documentElement();

    QDomNode dataPluginNode =
        mDocDm->elementsByTagName("dataPlugin").item(0);
    return dataPluginNode.attributes().namedItem("name").nodeValue();
}

void
vleDmDD::setCurrentTab(QString tabName)
{
    undoStackDm->current_source = tabName;
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

    undoStackDm->clear();
}

void
vleDmDD::undo()
{
    waitUndoRedoDm = true;
    undoStackDm->undo();
    emit modified();

}

void
vleDmDD::redo()
{
    waitUndoRedoDm = true;
    undoStackDm->redo();
    emit modified();
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


}}//namespaces
