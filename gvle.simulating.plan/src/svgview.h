/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2017-2017 INRA http://www.inra.fr
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
#ifndef SVGVIEW_H
#define SVGVIEW_H

#include <QGraphicsView>
#include <QSvgRenderer>
#include <QtXml>
#include <QMouseEvent>
#include <QWheelEvent>
#include "vlePlan.h"

class SvgViewConfig
{
public:
    SvgViewConfig() { }
    QString getName(void)      { return mName; }
    QString getKey (QString k) { return mConfig.value(k); }
    void    setName(QString v) { mName = v; }
    void    setKey (QString k, QString v) { mConfig.insert(k, v); }
    void removeKey (QString k) { mConfig.remove(k); }
private:
    QString mName;
    QMap<QString, QString> mConfig;
};

class SvgView: public QGraphicsView
{
    Q_OBJECT

public:
    SvgView(QWidget *parent = 0);
    void convert (const QString &xsltFile);
    QString getTplHeader(void);
    QString getTplTask  (void);
    QString getTplTime  (void);
    void loadPlan(vlePlan *plan);
    void loadFile(QString fileName);
    bool loadTemplate();
    bool loadTemplate(QString fileName);
    void refresh (void);
    void reload  (void);
    QString getConfig(QString c, QString key);
    void    setConfig(QString c, QString key, QString value);
    void setZommFactor(qreal factor);
private:
    void updateAttr (QDomNode    &e, QString selector,
		     QString attr, QString value, bool replace = true);
    void updateField(QDomNode    &e, QString tag,  QString value);
    void updatePos  (QDomElement &e, int x, int y);
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent* event);
private:
    // Widget variables
    QGraphicsItem *mGraphicItem;
    QSvgRenderer  *mSvgRenderer;
    // SVG template variables
    QDomDocument   mTplDocument;
    QDomElement    mTplRoot;
    QDomElement    mTplHeader;
    QDomElement    mTplTask;
    QDomElement    mTplTime;
    //
    vlePlan       *mPlan;
    int            mMaxWidth;
    qreal          mPixelPerDay;
    qreal          mZoomFactor;
    qreal          mZoomLevel;
    int            mGroupHeight;

    QList<SvgViewConfig *> mConfig;

    // Debug and temporary
    QString       mFilename;
};

#endif // SVGVIEW_H
