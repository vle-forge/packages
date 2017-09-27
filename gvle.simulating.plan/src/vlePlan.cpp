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
#include <QFile>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QDebug>
#include "vlePlan.h"

vlePlan::vlePlan()
{
    mValid = false;
    mGroups.clear();
}

void vlePlan::clear(void)
{
    // Delete all known groups
    while ( ! mGroups.isEmpty())
        delete mGroups.takeFirst();
    // Mark current plan as invalid
    mValid = false;
}

QDate vlePlan::dateEnd(void)
{
    if ( mDateEnd.isValid() )
        return mDateEnd;

    QDate lastest;

    for (int i = 0; i < mGroups.count(); i++)
    {
        vlePlanGroup *g = mGroups.at(i);

        // If the reference is not already set
        if ( ! lastest.isValid())
        {
            // Use the start date of this group as reference
            lastest = g->dateEnd();
            continue;
        }

        // If the end of this group is after the reference date
        if (g->dateEnd() > lastest)
            // Update to the lastest
            lastest = g->dateEnd();
    }

    // Save the start date (cache)
    mDateEnd = lastest;

    return mDateEnd;
}

QDate vlePlan::dateStart(void)
{
    if ( mDateStart.isValid() )
        return mDateStart;

    QDate oldest;

    for (int i = 0; i < mGroups.count(); i++)
    {
        vlePlanGroup *g = mGroups.at(i);

        // If the reference is not already set
        if ( ! oldest.isValid())
        {
            // Use the start date of this group as reference
            oldest = g->dateStart();
            continue;
        }

        // If the start of this group is before the reference date
        if (g->dateStart() < oldest)
            // Update to the oldest
            oldest = g->dateStart();
    }

    // Save the start date (cache)
    mDateStart = oldest;

    return mDateStart;
}

int vlePlan::countGroups(void)
{
    return mGroups.count();
}
int vlePlan::countActivities(void)
{
    int count = 0;
    for (int i = 0; i < mGroups.size(); ++i)
    {
        vlePlanGroup *a = mGroups.at(i);
        count += a->count();
    }
    return count;
}

void vlePlan::loadString(QString plan)
{
    int lineCount = 0;
    bool hasClass = false;
    int  attrCount = 0;
    QTextStream in(&plan, QIODevice::ReadOnly);

    clear();

    while (!in.atEnd())
    {
        // Get one line of text from CSV
        QString line = in.readLine();
        // Split the string to get CSV columns into a list
        QStringList fields = line.split(QChar(';'));

        if (lineCount == 0)
        {
            // If the header line is malformed, abort file load
            if (fields.count() < 4)
                break;
            if (fields.count() > 4)
            {
                hasClass = true;
                attrCount = (fields.count() - 5);
            }
            // Clear current plan (if any previously loaded)
            // clear();
            // lineCount++;
            // continue;
        }
        // Sanity check
        if (fields.count() < 4)
            continue;

        QString type;
        QString startDate;
        QString endDate;
        if (hasClass)
        {
            type      = fields.at(2);
            startDate = fields.at(3);
            endDate   = fields.at(4);
        }
        else
        {
            startDate = fields.at(2);
            endDate   = fields.at(3);
        }

        // Save the activity into the vlePlan
        vlePlanGroup    *g = getGroup(fields.at(1), true);
        vlePlanActivity *a = g->addActivity(fields.at(0));
        a->setClass(type);
        a->setStart(QDate::fromString(startDate, Qt::ISODate));
        a->setEnd  (QDate::fromString(endDate,   Qt::ISODate));

        // Process additional attributes
        for (int j = 0; j < attrCount; j++)
        {
            QString attr( fields.at(j + 5) );
            a->addAttribute(attr);
        }

        lineCount++;
    }

    for (int i = 0; i < mGroups.size(); i++)
    {
        vlePlanGroup *grp = mGroups.at(i);
        grp->sort();
    }

    // If (at least) one group has been loaded ...
    if (countGroups() > 0)
        // ... then, Plan is now valid
        mValid = true;
}

void vlePlan::loadFile(const QString &filename)
{
    QFile file(filename);
    int lineCount;
    bool hasClass = false;
    int  attrCount = 0;

    file.open(QIODevice::ReadOnly);

    lineCount = 0;

    while ( ! file.atEnd() )
    {
        // Get one line of text from CSV
        QByteArray line = file.readLine();
        // Split the string to get CSV columns into a list
        QStringList fields = QString(line).split(QChar(';'));

        if (lineCount == 0)
        {
            // If the header line is malformed, abort file load
            if (fields.count() < 4)
                break;
            if (fields.count() > 4)
            {
                hasClass = true;
                attrCount = (fields.count() - 5);
            }
            // Clear current plan (if any previously loaded)
            clear();
            lineCount++;
            continue;
        }
        // Sanity check
        if (fields.count() < 4)
            continue;

        QString type;
        QString startDate;
        QString endDate;
        if (hasClass)
        {
            type      = fields.at(2);
            startDate = fields.at(3);
            endDate   = fields.at(4);
        }
        else
        {
            startDate = fields.at(2);
            endDate   = fields.at(3);
        }

        // Save the activity into the vlePlan
        vlePlanGroup    *g = getGroup(fields.at(1), true);
        vlePlanActivity *a = g->addActivity(fields.at(0));
        a->setClass(type);
        a->setStart(QDate::fromString(startDate, Qt::ISODate));
        a->setEnd  (QDate::fromString(endDate,   Qt::ISODate));

        // Process additional attributes
        for (int j = 0; j < attrCount; j++)
        {
            QString attr( fields.at(j + 5) );
            a->addAttribute(attr);
        }

        lineCount++;
    }
    file.close();

    for (int i = 0; i < mGroups.size(); i++)
    {
        vlePlanGroup *grp = mGroups.at(i);
        grp->sort();
    }

    // If (at least) one group has been loaded ...
    if (countGroups() > 0)
        // ... then, Plan is now valid
        mValid = true;
}

vlePlanGroup *vlePlan::getGroup(QString name, bool create)
{
    vlePlanGroup *ret = NULL;

    for (int i = 0; i < mGroups.size(); ++i)
    {
        vlePlanGroup *a = mGroups.at(i);
        if (a->getName() == name)
        {
            ret = a;
            break;
        }
    }
    if ( (ret == NULL) && create)
    {
        ret = new vlePlanGroup(name);
        mGroups.push_back(ret);

        // Reset cache to NULL date
        mDateEnd   = QDate();
        mDateStart = QDate();
    }
    return ret;
}

vlePlanGroup *vlePlan::getGroup(int pos)
{
    if (pos > mGroups.count())
        return NULL;

    return mGroups.at(pos);
}

bool vlePlan::isValid(void)
{
    return mValid;
}

// ******************** Activities ******************** //

vlePlanActivity::vlePlanActivity(QString name)
{
    mName = name;
}
vlePlanActivity::~vlePlanActivity()
{
    // Nothing to do
}

void vlePlanActivity::addAttribute(QString value)
{
    mAttributes.append(value);
}

int vlePlanActivity::attributeCount(void)
{
    return mAttributes.size();
}

QDate vlePlanActivity::dateEnd(void)
{
    return mDateEnd;
}

QDate vlePlanActivity::dateStart(void)
{
    return mDateStart;
}

QString vlePlanActivity::getAttribute(int pos)
{
    if ((pos + 1) > mAttributes.size())
        return QString();

    return mAttributes.at(pos);
}

QString vlePlanActivity::getClass(void)
{
    return mClass;
}

QString vlePlanActivity::getName(void)
{
    return mName;
}

void vlePlanActivity::setClass(QString c)
{
    mClass = c;
}

void vlePlanActivity::setName(QString name)
{
    mName = name;
}

void vlePlanActivity::setStart(QDate date)
{
    mDateStart = date;
}

void vlePlanActivity::setEnd(QDate date)
{
    mDateEnd = date;
}

// ******************** Groups ******************** //

vlePlanGroup::vlePlanGroup(QString name)
{
    mName = name;
    mActivities.clear();
}

vlePlanGroup::~vlePlanGroup()
{
    // Delete all known activities
    while ( ! mActivities.isEmpty())
        delete mActivities.takeFirst();
}

QDate vlePlanGroup::dateEnd(void)
{
    if ( mDateEnd.isValid() )
        return mDateEnd;

    QDate lastest;

    for (int i = 0; i < mActivities.count(); i++)
    {
        vlePlanActivity *a = mActivities.at(i);

        // If the reference is not already set
        if ( ! lastest.isValid())
        {
            // Use the start date of this activity as reference
            lastest = a->dateEnd();
            continue;
        }

        // If the end of this activity is after the reference date
        if (a->dateEnd() > lastest)
            // Update to the lastest
            lastest = a->dateEnd();
    }

    // Save the start date (cache)
    mDateEnd = lastest;

    return mDateEnd;
}

QDate vlePlanGroup::dateStart(void)
{
    if ( mDateStart.isValid() )
        return mDateStart;

    QDate oldest;

    for (int i = 0; i < mActivities.count(); i++)
    {
        vlePlanActivity *a = mActivities.at(i);

        // If the reference is not already set
        if ( ! oldest.isValid())
        {
            // Use the start date of this activity as reference
            oldest = a->dateStart();
            continue;
        }

        // If the start of this activity is before the reference date
        if (a->dateStart() < oldest)
            // Update to the oldest
            oldest = a->dateStart();
    }

    // Save the start date (cache)
    mDateStart = oldest;

    return mDateStart;
}

QString vlePlanGroup::getName(void)
{
    return mName;
}
void vlePlanGroup::setName(QString name)
{
    mName = name;
}

int vlePlanGroup::count(void)
{
    return mActivities.count();
}

vlePlanActivity *vlePlanGroup::addActivity(QString name)
{
    vlePlanActivity *newAct;

    newAct = new vlePlanActivity(name);

    // Insert it to the list of known activities
    mActivities.push_back(newAct);

    // Reset cache to NULL date
    mDateEnd   = QDate();
    mDateStart = QDate();

    return newAct;
}

vlePlanActivity *vlePlanGroup::getActivity(int pos)
{
    if (pos > mActivities.count())
        return NULL;

    return mActivities.at(pos);
}

bool sortActivityDate(vlePlanActivity *v1, vlePlanActivity *v2)
{
    return v1->dateStart() < v2->dateStart();
}

void vlePlanGroup::sort(void)
{
    std::sort(mActivities.begin(), mActivities.end(), sortActivityDate);
}
