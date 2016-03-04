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
#ifndef GVLE_VLEOMOD_H
#define GVLE_VLEOMOD_H

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
 * @brief Class that implements vleDomObject especially for vleOm
 */
class vleDomOmOD : public vleDomObject
{
public:
    vleDomOmOD(QDomDocument* doc);
    ~vleDomOmOD();
    QString  getXQuery(QDomNode node);
    QDomNode getNodeFromXQuery(const QString& query, QDomNode d=QDomNode());
};

class vleOmOD : public QObject
{
    Q_OBJECT
public:
    vleOmOD(const QString& srcpath, const QString& smpath,
            QString pluginName);
    QString getSrcPath() const
    {return mFileNameSrc;};
    QString getOmPath() const
    {return mFileNameOm;};
    void setSrcPath(const QString& name)
    {mFileNameSrc = name;};
    void setOmPath(const QString& name)
    {mFileNameOm = name;};
    QString toQString(const QDomNode& node) const;
    void xCreateDom();
    void xReadDom();
    QString getOutPlugin();

    void setCurrentTab(QString tabName);

    void save();

    void undo();
    void redo();

private:
    QDomNode nodeVariable(const QString& varName);

public slots:
    void onUndoRedoOm(QDomNode oldValOm, QDomNode newValOm);
    void onUndoAvailable(bool);
signals:
    void undoRedo(QDomNode oldValOm, QDomNode newValOm);
    void undoAvailable(bool);

    void modified();


private:


    QDomDocument*    mDocOm;
    QString          mFileNameSrc;
    QString          mFileNameOm;
    vleDomOmOD*      mVdoOm;
    vleDomDiffStack* undoStackOm;
    bool             waitUndoRedoOm;
    QDomNode         oldValOm;
    QDomNode         newValOm;
    QString          mpluginName;
};

}}//namespaces

#endif
