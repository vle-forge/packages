/*
 * @file vle/extension/dsdevs/DSDevs.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef VLE_EXTENSION_DSDEVS_DSDEVS_HPP
#define VLE_EXTENSION_DSDEVS_DSDEVS_HPP

#include <vle/devs/Simulator.hpp>
#include <vle/devs/Executive.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/value/Value.hpp>

#include <list>

namespace vle { namespace extension {

   /**
    * Barros DEVS extension to provide graph manipulation at runtime.
    *
    *
    * Input ports:
    *
    * - addModel: add a model to the current coupled model. The model name
    *   must have a prefix, a class and two XML to initialise dynamic and
    *   init ports. The xmlInits, xmlDynamics and addConnection are not
    *   necessarry parameters. Set addConnection parameter is a set of
    *   addConnection message, with an empty model name to specify the new
    *   model to connect. Message is a Map: (["prefixModelName", String],
    *   ["className", String], ["xmlInits", String] ["xmlDynamics", String]
    *   ["addConnection", Set < Map >]
    *
    * - removeModel: delete a model to the current coupled model. Message is
    *   a Map: (["modelName", String])
    *
    * - addConnection: add a connection between to model from
    *   the current coupled model. Message is a Map: (["srcModelName",
    *   String], ["srcPortName", String], ["dstModelName", String],
    *   ["dstPortName", String])
    *
    * - removeConnection: remove a connection from current coupled model.
    *   Connection can be input, output or internal. Message is a Map:
    *   (["srcModelName", String], ["srcPortName", String], ["dstModelName",
    *   String], ["dstPortName", String])
    *
    * - changeConnection: to change a connection from an a model to an
    *   another. Only the the tuples model, port destination is modified.
    *   Message is a Map: (["srcModelName", String], ["srcPortName", String]
    *   ["oldDstModelName", String], ["oldDstPortName", String],
    *   ["newDstModelName", String], ["newDstPortName", String])
    *
    * - addInputPort: add an input port to the specified model.
    *   Message is a Map: (["modelName", String], ["portName", String])
    *
    * - addOutputPort: add an output port to the specified model.
    *   Message is a Map: (["modelName", String], ["portName", String])
    *
    * - removeInputPort: remove an input port to the specified
    *   model. Message is a Map: (["modelName", String], ["portName", String])
    *
    * - removeOutputPort: remove an output port to the specified
    *   model. Message is a Map: (["modelName", String], ["portName", String])
    *
    * - buildModel: build a new model, ie. compile a new
    *   dynamic library, load it, and instance a new model. The model name
    *   must have a prefix, a class and two XML to initialise dynamic and
    *   init ports. Message is a Map: (["prefixModelName", String],
    *   ["className", String], ["xmlCode", String], ["xmlInits", String],
    *   ["xmlDynamics", String])
    *
    * - changeModel: change the dynamic of a listing models or a list of
    *   dynamics. It consist into deleting the dynamics from atomic model, and
    *   attach a new dynamic. Message is a Map: TODO.
    *
    * - bag: a bag of simple messages to simplify building of message. Message
    *   is a Map of Set: (["bag" [ [Set "String"]]])
    *
    * Output ports:
    *
    * - name (Set ["msg", String])
    *     - Inform modeller to the modification results, if first parameter
    *     "result" is true, the graph manipulation is a success and "msg" is
    *     empty.  Otherwise, "result" is false, the "msg" string is fill with a
    *     description of error.
    *
    * - ok: ([Set Boolean])
    *     - Inform modeller that new models are building and/or adding. This
    *     parameter is a Set Value of String that represent the model name
    *     build by simulator.
    *
    * State ports:
    *  - coupled: (String), only models in the coupled model.
    *  - hierarchy: (String), the hierarchy of the coupled model.
    *  - complete: (String), complete hierarchy from top model.
    *
    */
    class DSDevs : public devs::Executive
    {
    public:
        enum state { IDLE, ADD_MODEL, REMOVE_MODEL, CHANGE_MODEL, BUILD_MODEL,
            ADD_CONNECTION, REMOVE_CONNECTION, CHANGE_CONNECTION, ADD_INPUTPORT,
            REMOVE_INPUTPORT, ADD_OUTPUTPORT, REMOVE_OUTPUTPORT, BAG };

        DSDevs(const devs::ExecutiveInit& model,
               const devs::InitEventList& events);

        virtual ~DSDevs() { }

        virtual devs::Time init(const devs::Time& time);

        virtual void output(
            const devs::Time& /* time */,
            devs::ExternalEventList& /* output */) const;

        virtual devs::Time timeAdvance() const;

        virtual void confluentTransitions(
            const devs::Time& /* time */,
            const devs::ExternalEventList& /* extEventlist */);

        virtual void internalTransition(const devs::Time& /* event */);

        virtual void externalTransition(
            const devs::ExternalEventList& /* event */,
            const devs::Time& /* time */);

        virtual value::Value* observation(
            const devs::ObservationEvent& /* event */) const;

        /*
         *
         * Static functions to build a single message.
         *
         */

        static value::Map* buildMessageAddConnection(
            const std::string& srcModelName,
            const std::string& srcPortName,
            const std::string& dstModelName,
            const std::string& dstPortName);

        static value::Map* buildMessageChangeConnection(
            const std::string& srcModelName,
            const std::string& srcPortName,
            const std::string& oldDstModelName,
            const std::string& oldDstPortName,
            const std::string& newDstModelName,
            const std::string& newDstPortName);

        static value::Map* buildMessageRemoveConnection(
            const std::string& srcModelName,
            const std::string& srcPortName,
            const std::string& dstModelName,
            const std::string& dstPortName);

        static value::Map* buildMessageAddModel(
            const std::string& prefixModelName,
            const std::string& className,
            const std::string& xmlDynamics,
            const std::string& xmlInits,
            value::Set* connection);

        static value::Map* buildMessageRemoveModel(
            const std::string& modelName);

        static value::Map* buildMessageChangeModel(
            const std::string& modelName,
            const std::string& className,
            const std::string& newClassName);

        static value::Map* buildMessageBuildModel(
            const std::string& prefixModelName,
            const std::string& className,
            const std::string& xmlCode,
            const std::string& xmlDynamics,
            const std::string& xmlInits);

        static value::Map* buildMessageAddInputPort(
            const std::string& modelName,
            const std::string& portName);

        static value::Map* buildMessageAddOutputPort(
            const std::string& modelName,
            const std::string& portName);

        static value::Map* buildMessageRemoveInputPort(
            const std::string& modelName,
            const std::string& portName);

        static value::Map* buildMessageRemoveOutputPort(
            const std::string& modelName,
            const std::string& portName);

        /*
         *
         * Static functions to build a Set of single function.
         *
         */

        static value::Set* addToBagAddConnection(
            const std::string& srcModelName,
            const std::string& srcPortName,
            const std::string& dstModelName,
            const std::string& dstPortName,
            value::Set* currentbag);

        static value::Set* addToBagChangeConnection(
            const std::string& srcModelName,
            const std::string& srcPortName,
            const std::string& oldDstModelName,
            const std::string& oldDstPortName,
            const std::string& newDstModelName,
            const std::string& newDstPortName,
            value::Set* currentbag);

        static value::Set* addToBagRemoveConnection(
            const std::string& srcModelName,
            const std::string& srcPortName,
            const std::string& dstModelName,
            const std::string& dstPortName,
            value::Set* currentbag);

        static value::Set* addToBagAddModel(
            const std::string& prefixModelName,
            const std::string& className,
            const std::string& xmlDynamics,
            const std::string& xmlInits,
            value::Set* connection,
            value::Set* currentbag);

        static value::Set* addToBagRemoveModel(
            const std::string& modelName,
            value::Set* currentbag);

        static value::Set* addToBagChangeModel(
            const std::string& modelName,
            const std::string& className,
            const std::string& newClassName,
            value::Set* currentbag);

        static value::Set* addToBagBuildModel(
            const std::string& prefixModelName,
            const std::string& className,
            const std::string& xmlCode,
            const std::string& xmlDynamics,
            const std::string& xmlInits,
            value::Set* currentbag);

        static value::Set* addToBagAddInputPort(
            const std::string& modelName,
            const std::string& portName,
            value::Set* currentbag);

        static value::Set* addToBagAddOutputPort(
            const std::string& modelName,
            const std::string& portName,
            value::Set* currentbag);

        static value::Set* addToBagRemoveInputPort(
            const std::string& modelName,
            const std::string& portName,
            value::Set* currentbag);

        static value::Set* addToBagRemoveOutputPort(
            const std::string& modelName,
            const std::string& portName,
            value::Set* currentbag);

    protected:
        bool addConnectionT(const std::string& srcModelName,
                            const std::string& srcPortName,
                            const std::string& dstModelName,
                            const std::string& dstPortName);
        bool changeConnectionT(const std::string& srcModelName,
                               const std::string& srcPortName,
                               const std::string& oldDstModelName,
                               const std::string& oldDstPortName,
                               const std::string& newDstModelName,
                               const std::string& newDstPortName);

        bool removeConnectionT(const std::string& srcModelName,
                               const std::string& srcPortName,
                               const std::string& dstModelName,
                               const std::string& dstPortName);

        bool addModelT(const std::string& prefixModelName,
                       const std::string& className,
                       const value::Set* connection);

        bool removeModelT(const std::string& modelName);

        bool changeModelT(const std::string& modelName,
                          const std::string& className,
                          const std::string& newClassName);

        bool buildModelT(const std::string& prefixModelName,
                         const std::string& className,
                         const std::string& xmlCode,
                         const std::string& xmlDynamics,
                         const std::string& xmlInits);

        bool addInputPortT(const std::string& modelName,
                           const std::string& portName);

        bool addOutputPortT(const std::string& modelName,
                            const std::string& portName);

        bool removeInputPortT(const std::string& modelName,
                              const std::string& portName);

        bool removeOutputPortT(const std::string& modelName,
                               const std::string& portName);

        const vpz::ModelList& getModelList() const;

        state                       m_state;
        std::list < std::string >   m_nameList;
        std::list < std::string >   m_newName;
        std::list < bool >          m_response;
    private:
        bool processSwitch(const std::string& action, const value::Map& val);
        bool processAddModel(const value::Map& val);
        bool processRemoveModel(const value::Map& event);
        bool processChangeModel(const value::Map& event);
        bool processBuildModel(const value::Map& event);
        bool processAddInputPort(const value::Map& event);
        bool processRemoveInputPort(const value::Map& event);
        bool processAddOutputPort(const value::Map& event);
        bool processRemoveOutputPort(const value::Map& event);
        bool processAddConnection(const value::Map& event);
        bool processRemoveConnection(const value::Map& event);
        bool processChangeConnection(const value::Map& event);
        bool processBag(const value::Map& event);
    };

}} // namespace vle extension

#endif
