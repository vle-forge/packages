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
#ifndef GVLE_VLESMFORRESTER_H
#define GVLE_VLESMFORRESTER_H

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

namespace gvle {
namespace forrester {

enum MODIF_TYPE
{
    RENAME, //modif related to the renaming of an object
    ADVANCED_EDTION,
    DEPENDENCY,
    NUMERICAL_INT, //modif concerning numerical integration part
    MOVE_OBJ, //modif concerning moving an obect
    OTHER
};

/**
 * @brief Class that implements DomObject especially for vleSmForrester
 */
class vleDomSmForrester : public vle::gvle::DomObject
{
public:
    vleDomSmForrester(QDomDocument* doc);
    ~vleDomSmForrester();
    QString  getXQuery(QDomNode node);
    QDomNode getNodeFromXQuery(const QString& query, QDomNode d=QDomNode());
};

class vleSmForrester : public QObject
{
    Q_OBJECT
public:
    vleSmForrester(const QString& srcpath, const QString& smpath,
            QString pluginName);


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
     * @brief add a compartment
     * @param compName is the name of the compartment
     * @param pos is location of compartment
     * @note if the compartment already exist, nothing is done
     * By default the output port is provided
     */
    void addCompartmentToDoc(const QString& compName, QPointF pos);

    /**
    * @brief add a parameter
    * @param paramName is the name of the parameter flow
    * @param pos is location of material flow
    * @note if the parameter already exist, nothing is done
    * By default the condition port is added
    */
   void addParameterToDoc(const QString& paramName, QPointF pos);
    /**
    * @brief add a material flow
    * @param matName is the name of the material flow
    * @param pos is location of material flow
    * @note if the material flow already exist, nothing is done
    * By default the output port is provided
    */
   void addMaterialFlowToDoc(const QString& matName, QPointF pos);

    /**
     * @brief setPosition to compartment
     * @param compName is the name of the compartment
     * @param pos is location of compartment
     * @param snap, if true performs a snapshot
     */
    void setPositionToCompartment(const QString& compName, QPointF pos,
            bool snap=true);
    void setPositionToParam(const QString& paramName, QPointF pos,
            bool snap=true);
    void setPositionToMaterialFlow(const QString& matName, QPointF pos,
            bool snap=true);
    void setPositionToSoftLink(const QString& matName, const QString& compName,
            QPointF pos, bool snap=true);

    void setAdvancedEdition(const QString& matFlow);
    bool hasAdvancedEdition(const QString& matFlow);

    void setCompute(const QString& matFlow, const QString& computeBody);
    QString getCompute(const QString& matFlow);

    /**
     * @brief tells if a material flow depends on other objects
     * (either comp of param)
     * @param matFlowName , the material flow name
     * @param otherName, a compartment of parameter
     */
    bool usefullForFlow(QString matFlowName, QString otherName);

    /**
     * @brief specify a dependency between a material flow and
     * a compartment or a parameter
     * @param matFlowName , the material flow name
     * @param otherName, a compartment or parameter
     * @param val, set a dependency or not
     * @param centerPoint, center point of the material flow
     * @param snapshot, performs snapshot or not
     *
     */
    void setUsefullForFlow(QString matFlowName, QString otherName, bool val,
            QPointF centerPoint=QPointF(), bool snapshot=true);

    /**
     * @brief specify a hard link from a material flow to a compartment
     * a compartment or a parameter
     * @param matFlowName , the material flow name
     * @param compName, a compartment
     * @param val, set a hard link or not
     * @param snapshot, should a snapshot be performed ?
     */
    void setHardLinkTo(QString matFlowName, QString compName, bool val,
            bool snapshot=true);
    /**
     * @brief specify a hard link from  a compartment to a material flow
     * @param matFlowName , the material flow name (destination of the link)
     * @param compName, a compartment (source of the hard link)
     * @param val, set a hard link or not
     * @param snapshot, should a snapshot be performed ?
     */
    void setHardLinkFrom(QString matFlowName, QString compName, bool val,
            bool snapshot=true);
    QString hardLinkTo(QString matFlowName) const;
    QString hardLinkFrom(QString matFlowName) const;
    QSet<QString> linksToComp(QString compName) const;
    QSet<QString> linksFromComp(QString compName) const;

    /**
     * @brief get the numerical integration method
     */
    QString numericalIntegration() const;
    double numericalTimeStep() const;
    double numericalQuantum(QString compName) const;
    /**
     * @brief set numerical integration
     * @param method: either 'euler', 'rk4' or 'qss2'
     */
    void setNumericalIntegration(QString method, bool snap=true);
    void setNumericalTimeStep(double ts, bool snap=true);
    void setNumericalQuantum(QString compName, double qt, bool snap=true);

    /**
     * @brief Set the type: In, Out or In/0ut
     * @param varName is the name of the variable
     * @param type: In, Out or In/0ut
     * @param snap, if true, perfoms a snapshot
     */
    void setType(const QString& compName, const QString& type, bool snap=true);

    /**
     * @brief set, get, remove the initial value of a compartment or the
     * value of a parameter
     * @param varName is the name of the compartment or parameter
     * @param val the value should be a double
     * @param snap to manage if a snapshot is expected ore not
     * @nore check whether it should be in the definition or
     * in the configuration
     */
    void setInitialValue(const QString& varName,
            double val, bool snap = true);
    std::unique_ptr<vv::Value> getInitialValue(const QString& varName) const;
    void unsetInitialValue(const QString& varName, bool snap = true);

     /**
     * @brief rename everywhere the name of a avariable
     * @param oldVariableName the previous name
     * @param newVariableName the new one
     */
    void renameCompartmentToDoc(const QString& oldName,
            const QString& newName);
    void renameParameterToDoc(const QString& oldName,
            const QString& newName);
    void renameMaterialFlowToDoc(const QString& oldName,
            const QString& newName);
     /**
     * @brief provide a unique new compartment name
     * @note the new name
     */
    QString newCompartmentNameToDoc();
    QString newParameterNameToDoc();
    QString newMaterialFlowNameToDoc();
    /**
     * @brief check if a compartment name is already used
     * @param compName the name of
     * @return true if a compartment already exist
     */
    bool existCompToDoc(QString compName);
    bool existParameterToDoc(QString paramName);
    bool existMaterialFlowToDoc(QString matName);
    /**
     * @brief remove a compartment and all the related items
     * @param compName the name of
     */
    void rmCompartmentToDoc(const QString& compName);
    void rmMateriaFlowToDoc(const QString& matName);
    void rmParameterToDoc(const QString& paramName);

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

    QDomNodeList compartmentsFromDoc();
    QDomNodeList parametersFromDoc();
    QDomNodeList materialFlowsFromDoc()  const;

    QSet<QString> compartmentNames();
    QSet<QString> parameterNames();
    QSet<QString> materialFlowNames();

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

    QDomNode nodeComps() const;
    QDomNode nodeParameters() const;
    QDomNode nodeMaterialFlows() const;

    /**
     * @brief get the node associated to a compartment
     */
    QDomNode nodeComp(const QString& varName) const;
    /**
     * @brief get the node associated to a compartment
     */

    QDomNode nodeParameter(const QString& paramName) const;
    /**
     * @brief get the node associated to a variable
     * (either a compartment or a parameter)
     */
    QDomNode nodeVar(const QString& varName) const;
    QDomNode nodeMaterialFlow(const QString& matName) const;

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

    void modified(int modifType);

private:

    QDomDocument*            mDocSm;
    QString                  mFileNameSrc;
    QString                  mFileNameSm;
    vleDomSmForrester*       mVdoSm;
    vle::gvle::DomDiffStack* undoStackSm;
    bool                     waitUndoRedoSm;
    QDomNode                 oldValSm;
    QDomNode                 newValSm;
    QString                  mpluginName;
};

}}//namespaces

#endif
