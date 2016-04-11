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
#ifndef GVLE_VLESMDT_H
#define GVLE_VLESMDT_H

#include <QLabel>
#include <QMouseEvent>
#include <QString>
#include <QWidget>
#include <QPainter>
#include <QPoint>
#include <QStaticText>
#include <QStyleOption>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNamedNodeMap>
#include <QXmlDefaultHandler>
#include <vle/value/Value.hpp>
#include <vle/value/Map.hpp>
#include <vle/gvle/vleDomDiffStack.h>

namespace vle {
namespace gvle {

/**
 * @brief Class that implements vleDomObject especially for vleSm
 */
class vleDomSmDT : public vleDomObject
{
public:
    vleDomSmDT(QDomDocument* doc);
    ~vleDomSmDT();
    QString  getXQuery(QDomNode node);
    QDomNode getNodeFromXQuery(const QString& query, QDomNode d=QDomNode());
};

class vleSmDT : public QObject
{
    Q_OBJECT
public:
    vleSmDT(const QString& srcpath, const QString& smpath,
            QString pluginName);
    QString getSrcPath() const
    {return mFileNameSrc;};
    QString getSmPath() const
    {return mFileNameSm;};
    void setSrcPath(const QString& name)
    {mFileNameSrc = name;};
    void setSmPath(const QString& name)
    {mFileNameSm = name;};
    QString toQString(const QDomNode& node) const;
    void xCreateDom();
    void xReadDom();
    QString getSrcPlugin();
    void addVariableToDoc(const QString& variableName);
    void setInitialValue(const QString& variableName,
            const vle::value::Value& val);
    void setPortCondValue(const QString& variableName,
            const vle::value::Value& val);
    vle::value::Value* getInitialValue(const QString& variableName);
    void rmInitialValue(const QString& variableName);
    void renameVariableToDoc(const QString& oldVariableName,
            const QString& newVariableName);
    QString newVarNameToDoc();
    bool existVarToDoc(QString varName);
    void rmVariableToDoc(const QString& variableName);

    void setComputeToDoc(const QString& computeBody);
    QString getComputeBody();
    void setClassNameToDoc(const QString& className);
    QString getClassName();
    void setNamespaceToDoc(const QString& nm);
    QString getNamespace();

    QDomNodeList variablesFromDoc();

    void setCurrentTab(QString tabName);

    void save();

    void undo();
    void redo();
    QString getData();

    // to factorize
    const QDomDocument& getDomDoc() const
    { return *mDocSm; }

    QDomDocument& getDomDoc()
    { return *mDocSm; }

    /**
     * @brief create a <dynamic> tag
     * whith dyn, attribute 'name'  set to dyn
     */
    QDomElement createDynamic();
    QDomElement createObservable();
    QDomElement createCondition();
    QDomElement createIn();
    QDomElement createOut();



private:
    QDomNode nodeVariable(const QString& varName);
    QDomNode nodeCondPort(const QString& portName);
    QDomNode nodeObsPort(const QString& portName);
    QDomNode nodeInPort(const QString& portName);
    QDomNode nodeOutPort(const QString& portName);

public slots:
    void onUndoRedoSm(QDomNode oldValSm, QDomNode newValSm);
    void onUndoAvailable(bool);
signals:
    void undoRedo(QDomNode oldValSm, QDomNode newValSm);
    void undoAvailable(bool);

    void modified();

private:

    QDomDocument*    mDocSm;
    QString          mFileNameSrc;
    QString          mFileNameSm;
    vleDomSmDT*      mVdoSm;
    vleDomDiffStack* undoStackSm;
    bool             waitUndoRedoSm;
    QDomNode         oldValSm;
    QDomNode         newValSm;
    QString          mpluginName;
};

}}//namespaces

#endif
