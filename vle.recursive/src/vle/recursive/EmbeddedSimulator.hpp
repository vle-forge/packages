/*
 * Copyright (C) 2014 INRA
 *
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

#ifndef VLE_RECURSIVE_EmbeddedSimulator_HPP_
#define VLE_RECURSIVE_EmbeddedSimulator_HPP_

#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Package.hpp>
#include <vle/manager/Manager.hpp>
#include <vle/value/Double.hpp>

#include <vle/recursive/VleAPIfacilities.hpp>
#include <vle/recursive/EmbeddedSimulatorInput.hpp>
#include <vle/recursive/EmbeddedSimulatorOutput.hpp>

//#include <boost/lexical_cast.hpp>

namespace vle {
namespace recursive {


namespace vu = vle::utils;
namespace vm = vle::manager;
namespace vz = vle::vpz;
namespace vv = vle::value;
namespace vo = vle::oov;

/**
 * @brief Class that implements an abstract simulator of
 * the simOptim library based on a VLE simulator.
 */
class EmbeddedSimulator
{

public:
    /**
     * @brief EmbeddedSimulator constructor
     */
    EmbeddedSimulator() : minputs(), mvpzDyn(), mmodules(), merror()
    {
    }

    /**
     * @brief Initialize the Simulator
     * @param initSim
     */
    virtual void init(const vv::Map& initSim)
    {
        const std::string& modelpkg = initSim.getString("package");
        const std::string& modelvpz = initSim.getString("vpz");
        vu::Package pkg(modelpkg);
        std::string vpzFile = pkg.getExpFile(modelvpz, vu::PKG_BINARY);
        mvpzDyn.parseFile(vpzFile);
        mvpzDyn.project().experiment().setCombination("linear");

        //handle inputs
        if(initSim.exist("inputs")){
            if (initSim.get("inputs")->isSet()) {
                const vv::Set& inputs = initSim.getSet("inputs");
                for(unsigned int i = 0; i< inputs.size(); i++){
                    minputs.push_back(EmbeddedSimulatorInput(
                            inputs.getString(i)));
                }
            } else {
                const std::string& input = initSim.getString("inputs");
                minputs.push_back(EmbeddedSimulatorInput(input));
            }
        }
        //handle outputs
        if(initSim.exist("outputs")){
            if (initSim.get("outputs")->isSet()) {
                const vv::Set& outputs = initSim.getSet("outputs");
                for(unsigned int i = 0; i< outputs.size(); i++){
                    moutputs.push_back(EmbeddedSimulatorOutput(outputs.getString(i)));
                }
            } else {
                const vv::String& output = initSim.getString("outputs");
                moutputs.push_back(EmbeddedSimulatorOutput(output.value()));
            }
        }
        if (initSim.exist("views")) {
            const vv::Map& viewsConfig = initSim.getMap("views");
            vv::Map::const_iterator itb = viewsConfig.begin();
            vv::Map::const_iterator ite = viewsConfig.end();
            for (; itb!=ite; itb++) {
                const std::string& viewName = itb->first;
                const vv::Map& viewConf = itb->second->toMap();
                if (viewConf.exist("plugin")) {
                    VleAPIfacilities::changePlugin(mvpzDyn, viewName,
                            viewConf.getString("plugin"));
                }
                if (viewConf.exist("type")) {
                    VleAPIfacilities::changeViewType(mvpzDyn, viewName,
                            viewConf.getString("type"));
                }
            }
        } else {
            VleAPIfacilities::changeAllPlugin(mvpzDyn,"storage");
        }
        if(initSim.exist("output_views")){
            throw vu::ArgError(" TEMP Except EmbeddedSimulator ") ;
            if (initSim.get("output_views")->isSet()) {
                const vv::Set& output_views = initSim.getSet("output_views");
                VleAPIfacilities::changeAllPlugin(mvpzDyn,"dummy");
                for(unsigned int i = 0; i< output_views.size(); i++){
                    const std::string& view_name = output_views.getString(i);
                    VleAPIfacilities::changePlugin(mvpzDyn, view_name,
                            "storage");
                }
            } else if (initSim.get("output_views")->isMap()) {
                const vv::Map& output_views = initSim.getMap("output_views");
                VleAPIfacilities::changeAllPlugin(mvpzDyn,"dummy");
                vv::Map::const_iterator itb = output_views.begin();
                vv::Map::const_iterator ite = output_views.end();
                for(; itb != ite; itb++){
                    const std::string& view_name = itb->first;
                    const std::string& view_type =
                            itb->second->toString().value();
                    VleAPIfacilities::changePlugin(mvpzDyn, view_name,
                                                "storage");
                    VleAPIfacilities::changeViewType(mvpzDyn, view_name,
                                                view_type);
                }
            }
        }
    }


    /**
     * @brief EmbeddedSimulator destructor
     */
    virtual ~EmbeddedSimulator()
    {
    }

    const std::string& getOutputName(unsigned int i) const
    {
        return moutputs[i].complete_name;
    }

    /**
     * @brief Add a value on a port of a condition of the vpz
     * @param condName, the name of the condition
     * @param portName, the name of the condition port
     * @param value, the value to set on port portName of
     * the condition condName (it is copied)
     * @param toreplace, if true values on port are erased and replaced,
     * otherwise the value is added
     */
    void addCondValue(const std::string& condName, const std::string& portName,
               const vv::Value& value, bool toreplace)
    {
        vz::Conditions& conds = mvpzDyn.project().experiment().conditions();
        vz::Condition& cond = conds.get(condName);
        if (toreplace) {
            cond.del(portName);
            cond.add(portName);
        }
        cond.addValueToPort(portName,value);
    }

    /**
     * @brief Add a value on a port of a condition of the vpz
     * @param condName, the name of the condition
     * @param portName, the name of the condition port
     * @param value, the value to set on port portName of
     * the condition condName (the value is not copied)
     * @param toreplace, if true values on port are erased and replaced,
     * otherwise the value is added
     */
    void addCondValue(const std::string& condName, const std::string& portName,
               vv::Value* value, bool toreplace)
    {
        vz::Conditions& conds = mvpzDyn.project().experiment().conditions();
        vz::Condition& cond = conds.get(condName);
        if (toreplace) {
            cond.del(portName);
            cond.add(portName);
        }
        cond.addValueToPort(portName,value);
    }

    /**
     * @brief get the vpz
     * @return a ref on the vpz
     */
    vz::Vpz& getVpz()
    {
        return mvpzDyn;
    }

protected:

    /**
     * @brief Clear all condition values
     */
    void clearAllCondPort()
    {
        vz::Conditions& conds = mvpzDyn.project().experiment().conditions();
        for(unsigned int i = 0; i < minputs.size();i++){
            const EmbeddedSimulatorInput& in = minputs[i];
            conds.get(in.cond_name).clearValueOfPort(in.port_name);
        }
    }
    /**
     *@brief Names of the inputs
     */
    std::vector<EmbeddedSimulatorInput> minputs;
    /**
     *@brief Names of the inputs
     */
    std::vector<EmbeddedSimulatorOutput> moutputs;
    /**
     *@brief vpz object representation, which defines
     *an experiment in vle (see vle)
     */
    vz::Vpz mvpzDyn;
    /**
     * @brief the module manager
     */
    vu::ModuleManager mmodules;
    /**
     * @brief Error structure for simulation
     */
    vm::Error merror;
};

}}//namespaces

#endif
