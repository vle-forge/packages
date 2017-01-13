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
#include <vle/gvle/dom_tools.hpp>

namespace vv = vle::value;

namespace vle {
namespace gvle {

/**
 * @brief Class that implements DomObject especially for vleSm
 */
class vleDomSmDT : public DomObject
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
    ~vleSmDT();

    QString getSrcPath() const
    {return mFileNameSrc;};
    QString getSmPath() const
    {return mFileNameSm;};
    void setSrcPath(const QString& name)
    {mFileNameSrc = name;};
    void setSmPath(const QString& name)
    {mFileNameSm = name;};
    void xCreateDom();
    void xReadDom();
    QString getSrcPlugin();
    void clearSnapStack();
     /**
     * @brief add a simple variable without history to the model
     * @param variableName is the name of the variable
     * @param vect, if true, add a vector
     * @note if the variable already exist, nothing is done
     * By default all the ports are provided to the conditions
     * And by default the is initial value is 0.0 and is not hardcode
     */
    void addVariableToDoc(const QString& variableName, bool vect);
    /**
     * @brief Tells is a variable is defined as a vector
     * @param varName is the name of the variable
     * @return true if varName is defined as a vector
     */
    bool isVect(const QString& varName) const;
    /**
     * @brief Tells is a variable is declared as input port
     * @param varName is the name of the variable
     * @return true if varName is declared as input port
     */
    bool isIn(const QString& varName) const;
    /**
     * @brief Tells is a variable is declared as output port
     * @param varName is the name of the variable
     * @return true if varName is declared as output port
     */
    bool isOut(const QString& varName) const;
    /**
     * @brief get the type: In, Out or In/0ut
     * @param varName is the name of the variable
     * @return the string representing the variable type
     */
    QString getType(const QString& varName) const;
    /**
     * @brief Set the type: In, Out or In/0ut
     * @param varName is the name of the variable
     * @param type: In, Out or In/0ut
     * @param snap, if true, perfoms a snapshot
     */
    void setType(const QString& varName, const QString& type, bool snap = true);

    /**
     * @brief Tells if a variable is observable
     * @param varName is the name of the variable
     * @return true if the variable is observable
     */
    bool isObs(const QString& varName) const;

    /**
     * @brief set the initial value
     * @param varName is the name of the variable
     * @param val the value can be a double or a tuple
     * @param snap to manage if a snapshot is expected ore not
     * @nore check whether it should be in the definition or
     * in the configuration
     */
    void setInitialValue(const QString& varName,
            const vv::Value& val, bool snap = true);
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
     * @param varName is the name of the variable
     * @param hsize the value of the history size
     * @param snap to manage if a snapshot is expected ore not
     */
    void setHistorySize(const QString& varName, int hsize,
            bool snap = true);
    /**
     * @brief set the dimension of a variable in the definition element
     * @param variableName is the name of the variable
     * @param dim , the dimension
     * @param snap to manage if a snapshot is expected ore not
     */
    void setDim(const QString& variableName, int dim, bool snap = true);


     /**
     * @brief set the sync condition to the configuration
     * @param varName is the name of the variable
     * @param val the value of the sync cond
     */
    void setSync(const QString& varName,int val);

    bool hasInitialValue(const QString& varName) const;
    std::unique_ptr<value::Value> getInitialValue(
            const QString& variableName) const;
    void setInitialDefaultValue(const QString& varName);
    void unsetInitialValue(const QString& varName);

    int getHistorySize(const QString& variableName);
    int getDim(const QString& variableName);
    int getSync(const QString& variableName);
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
    void rmVariableToDoc(const QString& varName);


    void addObsToDoc(const QString& varName);
    void rmObsToDoc(const QString& varName);

    void setComputeToDoc(const QString& computeBody);
    QString getComputeBody();
    void setConstructorToDoc(const QString& computeBody);
    QString getConstructorBody();
    void setIncludesToDoc(const QString& computeBody);
    QString getIncludesBody();
    void setUserSectionToDoc(const QString& computeBody);
    QString getUserSectionBody();
    void setClassNameToDoc(const QString& className, bool snap=true);
    QString getClassName();
    void setPackageToDoc(const QString& nm, bool snap=true);
    QString getPackage();

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
    QDomNode nodeVariables() const;
    QDomNode nodeVariable(const QString& varName) const;
    QDomNode nodeCond() const;
    QDomNode nodeCondPort(const QString& portName) const;
    QDomNode nodeObs() const;
    QDomNode nodeObsPort(const QString& portName);
    QDomNode nodeIn() const;
    QDomNode nodeOut() const;
    QDomNode nodeConfiguration() const;
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
    DomDiffStack*    undoStackSm;
    bool             waitUndoRedoSm;
    QDomNode         oldValSm;
    QDomNode         newValSm;
    QString          mpluginName;
};

}}//namespaces

#endif
