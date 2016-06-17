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

namespace vv = vle::value;

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
     /**
     * @brief add a simple variable without history to the model
     * @param variableName is the name of the variable
     * @note if the variable already exist, nothing is done
     * By default all the ports are provided to the conditions
     * And by default the is initial value is 0.0 and is not hardcode
     */
    void addVariableToDoc(const QString& variableName);
     /**
     * @brief set the initial value in the definition element
     * @param variableName is the name of the variable
     * @param val the value can be a double or a tuple
     * @param snap to manage if a snapshot is expected ore not
     */
    void setInitialDefValue(const QString& variableName,
			    const vv::Value& val,
			    const bool snap = true);
    /**
     * @brief set the initial value in the configuration element
     * @param variableName is the name of the variable
     * @param val the value can be a double or a tuple
     * @param snap to manage if a snapshot is expected ore not
     */
    void setInitialCondValue(const QString& variableName,
			     const vv::Value& val,
			     const bool snap = true);
    /**
     * @brief set the initial value
     * @param variableName is the name of the variable
     * @param val the value can be a double or a tuple
     * @param snap to manage if a snapshot is expected ore not
     * @nore check whether it should be in the definition or
     * in the configuration
     */
    void setInitialValue(const QString& variableName,
			 const vv::Value& val,
			 const bool snap = true);
     /**
     * @brief set the time step condition in the configuration element
     * @param val the value of the time step
     */
    void setTimeStep(vv::Double& val);
     /**
     * @brief remove the time step condition
     */
    void UnSetTimeStep();
     /**
     * @brief return the time step set in the condition
     * @return the time step
     */
    double getTimeStep();
     /**
     * @brief check if a time step is available as condition
     * @return true if a time step is set
     */
    bool hasTimeStep();
     /**
     * @brief set the history size of a variable in the definition element
     * @param variableName is the name of the variable
     * @param val the value of the history size
     * @param snap to manage if a snapshot is expected ore not
     */
    void setHistorySize(const QString& variableName,
			const vv::Value& val,
			const bool snap = true);
     /**
     * @brief set the history size and update the initial value
     * @param variableName is the name of the variable
     * @param histval the value of the history size
     * @param snap to manage if a snapshot is expected ore not
     * @note the structure depending of the context can change from double to
     * tuple and when resizing already existing values are keeped
     */
    void setHistorySizeAndValue(const QString& variableName,
				const vv::Value& histValue,
				const bool snap = true);

    /**
     * @brief set the dimension of a variable in the definition element
     * @param variableName is the name of the variable
     * @param val the value of the dimension
     * @param snap to manage if a snapshot is expected ore not
     */
    void setDim(const QString& variableName,
		const vv::Value& val,
		const bool snap = true);
    /**
     * @brief set the dimension  and update the initial value
     * @param variableName is the name of the variable
     * @param dimvalue the value of the dimension
     * @param snap to manage if a snapshot is expected ore not
     * @note the structure depending of the context can change from double to
     * tuple and when resizing already existing values are keeped
     */
    void setDimAndValue(const QString& variableName,
			const vv::Value& dimValue,
			const bool snap = true);
     /**
     * @brief move the initial value from the definition element
     * to the configuration or reverse
     * @param variableName is the name of the variable
     * @param parametrable if true the init is parametrable,
     * if not it is harcoded
     * @param snap to manage if a snapshot is expected ore not
     */
    void Parametrable(const QString& variableName,
		      const bool parametrable,
		      const bool snap = true);
     /**
     * @brief set the sync condition to the configuration
     * @param variableName is the name of the variable
     * @param val the value of the sync cond
     */
    void setSync(const QString& variableName,
		 const vv::Value& val);

    void setPortCondDoubleValue(const QString& variableName,
				const vv::Value& val);
    void setPortCondIntegerValue(const QString& variableName,
				 const vv::Value& val);
    void setPortCondBoolValue(const QString& variableName,
			      const vv::Value& val);
    void setPortCondTupleValue(const QString& portName,
                               const vv::Value& val);
    vv::Value* getInitialValue(const QString& variableName);
    vv::Value* getHistorySize(const QString& variableName);
    vv::Value* getDim(const QString& variableName);
    vv::Value* getSync(const QString& variableName);
     /**
     * @brief rename everywhere the name of a avariable
     * @param oldVariableName the previous name
     * @param newVariableName the new one
     */
    void renameVariableToDoc(const QString& oldVariableName,
			     const QString& newVariableName);
     /**
     * @brief provide a unique new variable name
     * @note the new name
     */
    QString newVarNameToDoc();
    /**
     * @brief check if a variable name is already used
     * @param variableName the name of
     * @return true if a variable already exist
     */
    bool existVarToDoc(QString varName);
    /**
     * @brief remove a variable and all the related items
     * @param variableName the name of
     */
    void rmVariableToDoc(const QString& variableName);
    void addInToDoc(const QString& variableName);
    void rmInToDoc(const QString& variableName);
    bool hasInFromDoc(const QString& variableName);
    void addOutToDoc(const QString& variableName);
    void rmOutToDoc(const QString& variableName);
    bool hasOutFromDoc(const QString& variableName);
    void addObsToDoc(const QString& variableName);
    void rmObsToDoc(const QString& variableName);
    bool hasObsFromDoc(const QString& variableName);
    /**
     * @brief check if a variable is parametrable
     * @param variableName the name of
     * @note so far only simple double variable can be hardcoded
     */
    bool isParametrable(const QString& variableName);

    void setComputeToDoc(const QString& computeBody);
    QString getComputeBody();
    void setConstructorToDoc(const QString& computeBody);
    QString getConstructorBody();
    void setIncludesToDoc(const QString& computeBody);
    QString getIncludesBody();
    void setUserSectionToDoc(const QString& computeBody);
    QString getUserSectionBody();
    void setClassNameToDoc(const QString& className);
    QString getClassName();
    void setNamespaceToDoc(const QString& nm);
    QString getNamespace();

    QDomNodeList variablesFromDoc();

    void setCurrentTab(QString tabName);

    void provideCpp();
    void save();

    void undo();
    void redo();
    QString getData();

    // to factorize
    const QDomDocument& getDomDoc() const
    { return *mDocSm; }

    QDomDocument& getDomDoc()
    { return *mDocSm; }

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
