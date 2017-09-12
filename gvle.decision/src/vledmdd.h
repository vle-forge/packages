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
#include <vle/gvle/dom_tools.hpp>

namespace vle {
namespace gvle {

enum MODIF_TYPE
{
    OUTPUTS,
    RENAME, //modif related to the renaming of an object
    RENAMERULE,
    RULES,
    RENAMEPREDICATE,
    PREDICATE,
    ADVANCED_EDTION,
    DEPENDENCY,
    MOVE_OBJ, //modif concerning moving an obect
    OTHER
};

/**
 * @brief Class that implements vleDomObject especially for vleDm
 */
class vleDomDmDD : public vle::gvle::DomObject
{
public:
    vleDomDmDD(QDomDocument* doc);
    ~vleDomDmDD();
    QString  getXQuery(QDomNode node);
    QDomNode getNodeFromXQuery(const QString& query, QDomNode d=QDomNode());
};

class vleDmDD : public QObject
{
    Q_OBJECT
public:
    vleDmDD(const QString& srcpath, const QString& smpath,
            QString pluginName);
    QString getSrcPath() const
    {return mFileNameSrc;};
    QString getDmPath() const
    {return mFileNameDm;};
    void setSrcPath(const QString& name)
    {mFileNameSrc = name;};
    void setDmPath(const QString& name)
    {mFileNameDm = name;};
    QString toQString(const QDomNode& node) const;
    void xCreateDom();
    void xReadDom();
    QString getDataModel();
    void setCurrentTab(QString tabName);

    void setClassNameToDoc(const QString& className, bool snap);
    void setPluginNameToDoc(const QString& plugName, bool snap);
    void setDataNameToDoc(const QString& dataName, const QString& pacName, bool snap);
    void setDataPackageToDoc(const QString& pacName, bool snap);
    void setPackageToDoc(const QString& nm, bool snap);

    QDomNodeList PrecedencesFromDoc();

    QString getPrecedenceType(const QString& firstName,
			     const QString& secondName);

    void setPrecedenceType(const QString& firstName,
			   const QString& secondName,
			   int idType);

    QString getPrecedenceMinLag(const QString& firstName,
				const QString& secondName);

    void setPrecedenceMinLag(const QString& firstName,
			     const QString& secondName,
			     int lag);

    QString getPrecedenceMaxLag(const QString& firstName,
				const QString& secondName);

    void setPrecedenceMaxLag(const QString& firstName,
			     const QString& secondName,
			     int lag);

    void addPrecedenceToDoc(const QString& firstName,
			    const QString& secondName);

    void rmPrecedenceToDoc(const QString& firstName,
			   const QString& secondName);

    void rmPrecedencesToDoc(const QString& activityName);

    bool existPrecedenceToDoc(const QString& firstName,
			      const QString& secondName);

    QDomNode nodePrecedences() const;
    QDomNode nodePrecedence(const QString& firstName,
			    const QString& secondName) const;

    QDomNodeList RulesFromDoc();

    void addRuleToDoc(const QString& ruleName);

    QString newRuleNameToDoc();

    void rmRuleToDoc(const QString& ruleName);

    /**
     * @brief rename everywhere the name of a rule
     * @param oldName the previous name
     * @param newName the new one
     */
    void renameRuleToDoc(const QString& oldName,
     			 const QString& newName);

    bool existRuleToDoc(const QString& ruleName);

    QDomNode nodeRules() const;
    QDomNode nodeRule(const QString& ruleName) const;

    QDomNodeList predicatesFromDoc();

    void addPredicateToDoc(const QString& predName);

    QString getPredicateOperator(const QString& predicateName);
    QString getPredicateLeftType(const QString& predicateName);
    QString getPredicateRightType(const QString& predicateName);
    QString getPredicateRightValue(const QString& predicateName);
    QString getPredicateLeftValue(const QString& predicateName);

    void setPredicateOperator(const QString& predicateName,
			      const QString& op,
			      bool snap = true);
    void setPredicateLeftType(const QString& predicateName,
			      const QString& lt,
			      bool snap = true);
    void setPredicateRightType(const QString& predicateName,
			      const QString& rt,
			      bool snap = true);
    void setPredicateRightValue(const QString& predicateName,
				const QString& type,
				const QString& olv,
				const QString& lv,
				bool snap = true);
    void setPredicateLeftValue(const QString& predicateName,
			       const QString& type,
                               const QString& orv,
			       const QString& rv,
			       bool snap = true);

    QString newPredicateNameToDoc();

    void rmPredicateToDoc(const QString& predName);

    bool singleVarUsage(const QString& varName);

    /**
     * @brief rename everywhere the name of a predicate
     * @param oldName the previous name
     * @param newName the new one
     */
    void renamePredicateToDoc(const QString& oldName,
			      const QString& newName);

    bool existPredToDoc(const QString& predName);

    QDomNode nodePreds() const;
    QDomNode nodePred(const QString& predName) const;

    QDomNodeList activitiesFromDoc();

    /**
     * @brief add a activity
     * @param compName is the name of the activity
     * @param pos is location of activity
     * @note if the activity already exist, nothing is done
     * By default the output port is provided
     */
    void addActivityToDoc(const QString& compName, QPointF pos);

    /**
     * @brief setPosition to activity
     * @param actName is the name of the activity
     * @param pos is location of activity
     * @param snap, if true performs a snapshot
     */
    void setPositionToActivity(const QString& actName, QPointF pos,
				  bool snap=true);

    bool isRelativeDate(const QString& actName);

    void setMaxIter(const QString& actName, const int maxIter,
		    bool snap=true);

    void setTimeLag(const QString& actName, const int timeLag,
		    bool snap=true);
    /**
     * @brief set minStart to activity
     * @param actName is the name of the activity
     * @param min is the min start date string
     * @param snap, if true performs a snapshot
     */
    void setMinStart(const QString& actName, const QString& min,
		     bool snap=true);

    int getMaxIter(const QString& actName);

    int getTimeLag(const QString& actName);

    QString getMinStart(const QString& actName);

    /**
     * @brief set maxfinish to activity
     * @param actName is the name of the activity
     * @param masx is the max fiish date string
     * @param snap, if true performs a snapshot
     */
    void setMaxFinish(const QString& actName, const QString& max,
		      bool snap=true);
    QString getMaxFinish(const QString& actName);

    /**
     * @brief rename everywhere the name of a avariable
     * @param oldVariableName the previous name
     * @param newVariableName the new one
     */
    void renameActivityToDoc(const QString& oldName,
			     const QString& newName);

    /**
     * @brief provide a unique new activity name
     * @note the new name
     */

    QString newActivityNameToDoc();
    /**
     * @brief check if a activity name is already used
     * @param compName the name of
     * @return true if a activity already exist
     */
    bool existActToDoc(QString actName);

     /**
     * @brief remove a activity and all the related items
     * @param actName the name of
     */
    void rmActivityToDoc(const QString& actName);

    QDomNodeList activitysFromDoc();

    bool singleOutUsage(const QString& outName);
    bool outUsed(const QString& outName);

    QString newOutParActNameToDoc();

    void addOutParActToDoc(const QString& actName,
			   const QString& outParName);
    void setValueOutParAct(const QString& actName,
			   const QString& outParName,
			   double value,
			   bool snap = true);
    void renameOutParActToDoc(const QString& actName,
			      const QString& oldOutParName,
                              const QString& newOutParName);
    QString newOutParToDoc();
    bool existOutParActToDoc(QString outParName);
    bool existOutParActToDoc(QString actName,
			     QString outParName);
    void rmOutputParActToDoc(const QString& actName,
			     const QString& OutParName);
    QDomNodeList outParActFromDoc(const QString& actName);
    QDomNode nodeOutParsAct(const QString& actName) const;
    QDomNode nodeOutParAct(const QString& actName,
			   const QString& outParName) const;

    void addRuleActToDoc(const QString& actName,
			 const QString& ruleName);
    bool existRuleActToDoc(QString actName,
			   QString ruleName);
    void rmRuleActToDoc(const QString& actName,
			const QString& ruleName);
    QDomNodeList ruleActFromDoc(const QString& actName);
    QDomNode nodeRulesAct(const QString& actName) const;
    QDomNode nodeRuleAct(const QString& actName,
			 const QString& ruleName) const;

    void addPredicateRuleToDoc(const QString& ruleName,
			       const QString& predicateName);

    bool existPredicateRuleToDoc(QString ruleName,
				 QString predicateName);
    void rmPredicateRuleToDoc(const QString& ruleName,
			      const QString& predicateName);

    QDomNode nodePredicateRule(const QString& ruleName,
			       const QString& predicateName) const;

    QSet<QString> activityNames();
    QSet<QString> rulesNames();
    QSet<QString> predicatesNamesRule(QString ruleName);

    void save();
    void providePlan();
    QString getData();

    void undo();
    void redo();

    // to factorize
    const QDomDocument& getDomDoc() const
    { return *mDocDm; }

    QDomDocument& getDomDoc()
    { return *mDocDm; }

    QDomNode nodeActs() const;

    /**
     * @brief get the node associated to an activity
     */
    QDomNode nodeAct(const QString& actName) const;

    QDomNode nodeCondPort(const QString& portName) const;

    QDomNode nodeOut() const;
    QDomNode nodeIn() const;
    QDomNode nodeCond() const;

private:
    QDomNode nodeVariable(const QString& varName);

public slots:
    void onUndoRedoDm(QDomNode oldValDm, QDomNode newValDm);
    void onUndoAvailable(bool);
signals:
    void undoRedo(QDomNode oldValDm, QDomNode newValDm);
    void undoAvailable(bool);

    void modified(int modifiedType);


private:


    QDomDocument*               mDocDm;
    QString                     mFileNameSrc;
    QString                     mFileNameDm;
    vleDomDmDD*                 mVdoDm;
    vle::gvle::DomDiffStack* undoStackDm;
    bool                        waitUndoRedoDm;
    QDomNode                    oldValDm;
    QDomNode                    newValDm;
    QString                     mpluginName;
};

}}//namespaces

#endif
