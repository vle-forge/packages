/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 20014-2014 INRA http://www.inra.fr
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


#ifndef _VLE_UTILS_PKG_TESTER_SIMULATION_HPP
#define _VLE_UTILS_PKG_TESTER_SIMULATION_HPP 1

#include <vle/vle.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Value.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/vpz/Model.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/utils/Context.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Package.hpp>
#include <vle/manager/Simulation.hpp>
#include <vle/manager/Manager.hpp>

namespace vle {
namespace tester {


namespace vz = vle::vpz;
namespace vu = vle::utils;
namespace vm = vle::manager;
namespace va = vle::value;

/**
 * @brief Provides high level functions for simulating a vpz from the current
 * package
 */
class TesterSimulation
{

public:
    /**
     * TesterSimulation constructor
     * @brief packagename, the name of the current package to test
     */
    TesterSimulation(const std::string& packagename,
            const std::string& vpzname, const utils::ContextPtr& ctx):
        mapp(), mvpz(), mCtx(ctx)
    {
        vu::Package pkg(mCtx, packagename);
        mvpz = std::unique_ptr<vz::Vpz>(new vz::Vpz(pkg.getExpFile(vpzname)));
    }



    ~TesterSimulation()
    {
    }

    /**
     * @brief Attaches to an atomic model a set of conditions
     * @param atomModel, the complete name of the model
     * @param conds, the set of conditions to attach to atomModel
     */
    void setConditions(const std::string& atomModel,
            const std::vector<std::string>& conds)
    {
        if(mvpz){
            vz::Model& vpz_mod = mvpz->project().model();
            vz::BaseModel* mdl = vpz_mod.model()->findModelFromPath(atomModel);
            vz::AtomicModel* atomg = mdl->toAtomic();
            atomg->setConditions(conds);
        }
    }
    /**
     *@brief Set all views to storage mode in order to get results.
     *TODO dependence on vle.output
     */
    void setStorageViews()
    {
        if(mvpz){
            //set all output plugin to storage
            vz::Outputs::iterator itb =
                    mvpz->project().experiment().views().outputs().begin();
            vz::Outputs::iterator ite =
                    mvpz->project().experiment().views().outputs().end();
            for(;itb!=ite;itb++) {
                va::Map* configOutput = new va::Map();
                //configOutput->addInt("rows",10000);
                //configOutput->addInt("inc_rows",10000);
                configOutput->addString("header","top");//A voir
                vz::Output& output = itb->second;
                output.setLocalStream("", "storage", "vle.output");
                output.setData(std::unique_ptr<value::Value>(configOutput));
            }
        }
    }
    /**
     * @brief Get all views for one simple simulation
     * @return the map corresponding the the output of the simulation
     */
    std::unique_ptr<va::Map> simulates()
    {
        if (mvpz) {
            setStorageViews();
            vm::Simulation sim(mCtx, vm::LOG_NONE, vm::SIMULATION_NONE, NULL);
            std::unique_ptr<va::Map> res = sim.run(std::move(mvpz), &merror);
            return res;
        } else {
            return std::unique_ptr<va::Map>();
        }
    }

    vm::Error& getError()
    {
        return merror;
    }

    va::Matrix& getView(va::Map& sim_outputs, const std::string& viewname)
    {
        if (! sim_outputs.exist(viewname)) {
            throw vu::ArgError(vle::utils::format(
                    " View not found : %s",
                    viewname.c_str()));
        }
        return sim_outputs.getMatrix(viewname);
    }


    const va::Value*  getColElt(va::Map& sim_outputs, const std::string& viewname,
            const std::string& colName,
            unsigned int i)
    {
        va::Matrix& view = getView(sim_outputs, viewname);
        if (view.rows() <= i) {
            throw vu::ArgError(vle::utils::format(
                    " index to big : %u",  i));
        }

        for(unsigned int j=1; j < view.columns(); j++){
            if(view.getString(j,0).compare(colName) == 0){
                return view.get(j,i).get();
            }
        }
        return 0;
    }

private:

    vle::Init mapp;
    std::unique_ptr<vz::Vpz> mvpz;
    vm::Error merror;
    utils::ContextPtr mCtx;
};

}}//namespaces
#endif
