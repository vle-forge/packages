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
#ifndef VLEPLAN_H
#define VLEPLAN_H

#include <QDate>
#include <QList>

class vlePlanActivity
{
public:
    vlePlanActivity (QString name);
    ~vlePlanActivity();
    void    addAttribute(QString value);
    int     attributeCount(void);
    QDate   dateEnd   (void);
    QDate   dateStart (void);
    QString getAttribute(int pos);
    QString getClass(void);
    QString getName (void);
    void    setClass(QString c);
    void    setName (QString name);
    void    setStart(QDate   date);
    void    setEnd  (QDate   date);
private:
    QString mName;
    QString mClass;
    QDate   mDateStart;
    QDate   mDateEnd;
    QList<QString> mAttributes;
};

class vlePlanGroup
{
public:
    vlePlanGroup   (QString name);
    ~vlePlanGroup  ();
    QDate   dateEnd  (void);
    QDate   dateStart(void);
    QString getName(void);
    void    setName(QString name);
    int     count(void);
    vlePlanActivity *addActivity(QString name);
    vlePlanActivity *getActivity(int pos);
    void    sort(void);
private:
    QDate   mDateEnd;    // Cache for the lastest "end date" of group activities
    QDate   mDateStart;  // Cache for the earliest "start date"of group activities
    QString mName;
    QList<vlePlanActivity *> mActivities;
};

class vlePlan
{
public:
    vlePlan();
    void  clear(void);
    QDate dateEnd  (void);
    QDate dateStart(void);
    void loadFile(const QString &filename);
    void loadString(QString plan);
    vlePlanGroup *getGroup(QString name, bool create = false);
    vlePlanGroup *getGroup(int pos);
    int  countGroups(void);
    int  countActivities(void);
    bool isValid(void);
private:
    bool  mValid;
    QDate mDateEnd;    // Cache for the start date
    QDate mDateStart;  // Cache for the end date
    QList<vlePlanGroup *> mGroups;
};

#endif // VLEPLAN_H
