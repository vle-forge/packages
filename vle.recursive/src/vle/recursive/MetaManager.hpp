
/*
 * Copyright (C) 2015 INRA
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

#ifndef VLE_RECURSIVE_METAMANAGER_HPP_
#define VLE_RECURSIVE_METAMANAGER_HPP_

#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <string>
#include <thread>
#include <memory>

#include <vle/devs/InitEventList.hpp>
#include <vle/value/Map.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Spawn.hpp>
#include <vle/manager/Simulation.hpp>
#include <vle/manager/Manager.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/Table.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/manager/Types.hpp>
#include <vle/utils/Context.hpp>
#include <vle/utils/Rand.hpp>
#include <vle/recursive/accu_multi.hpp>

#include "MetaManagerUtils.hpp"
#include "VleAPIfacilities.hpp"

namespace vle {
namespace recursive {

/**
 * @brief Class that implements a meta manager, ie an API for performing
 * simulation of experiment plans
 */
class MetaManager
{
private:
    std::string mIdVpz;
    std::string mIdPackage;
    CONFIG_PARALLEL_TYPE mConfigParallelType;
    bool mRemoveSimulationFiles;
    bool mUseSpawn;
    unsigned int mConfigParallelNbSlots;
    utils::Rand mrand;
    std::vector<std::unique_ptr<VleDefine>> mDefine;
    std::vector<std::unique_ptr<VlePropagate>> mPropagate;
    std::vector<std::unique_ptr<VleInput>> mInputs;
    std::vector<std::unique_ptr<VleReplicate>> mReplicates;
    std::vector<std::unique_ptr<VleOutput>> mOutputs;//view * port
    std::vector<std::unique_ptr<value::Value>>
      mOutputValues;//values are Tuple or Set
    std::string mWorkingDir; //only for cvle
    utils::ContextPtr mCtx;

public:

    /**
     * @brief MetaManager constructor
     */
    MetaManager();
    MetaManager(utils::ContextPtr ctx);

    /**
     * @brief MetaManager destructor
     */
    virtual ~MetaManager();

    utils::Rand& random_number_generator();

    /**
     * @brief Simulates the experiment plan
     * @param[in] init, the initialization map
     * @param[out] init, an error structure
     * @return the simulated values
     */
    inline std::unique_ptr<value::Map> run(
            std::shared_ptr<vle::value::Map> init,
            manager::Error& err)
    {
        wrapper_init init_rec(init.get());
        return run(init_rec, err);
    }

    inline std::unique_ptr<value::Map> run(
            const vle::value::Map& init,
            manager::Error& err)
    {
        wrapper_init init_rec(&init);
        return run(init_rec, err);
    }


    inline std::unique_ptr<value::Map> run(
            const vd::InitEventList& events,
                manager::Error& err)
    {
        wrapper_init init_rec(&events);
        return run(init_rec, err);
    }

private:

    unsigned int inputsSize() const;
    unsigned int replicasSize() const;

    void produceCvleInFile(const wrapper_init& init, const std::string& inPath,
            manager::Error& err);

    /**
     * @brief read header of an id in cvle output file
     *
     * @param outFile, ifsream of the cvle output file
     * @param line, last line read (to avoid new allocation)
     * @param tokens, last token split (to avoid new allocation)
     * @param[out] inputId, id of the input
     * @param[out] inputRepl, id of the replicate
     *
     * @return true if header has been correctly parsed, false otherwise
     *
     * @note example of lines to read:
     *
     *  id_10_2
     *
     */
    bool readCvleIdHeader(std::ifstream& outFile,
                std::string& line, std::vector <std::string>& tokens,
                int& inputId, int& inputRepl);

    /**
     * @brief read header of a view in cvle output file
     *
     * @param outFile, ifsream of the cvle output file
     * @param line, last line read (to avoid new allocation)
     * @param tokens, last token split (to avoid new allocation)
     * @param[out] viewName, the name of the view read or empty
     * if an error occurred
     *
     * @note example of lines to read:
     *
     * @return true if header has been correctly parsed, false otherwise
     *
     *  view:viewNoise
     *
     */
    bool readCvleViewHeader(std::ifstream& outFile,
            std::string& line, std::vector <std::string>& tokens,
            std::string& viewName);

    /**
     * @brief read a matrix form the clve output file
     *
     * @param outFile, ifsream of the cvle output file
     * @param line, last line read (to avoid new allocation)
     * @param tokens, last token split (to avoid new allocation)
     * @param nb_rows, insight of the number of rows into the matrix
     *
     * @return the matrix filled with read values
     *
     * @note example of lines to read:
     *
     * time ExBohachevsky:ExBohachevsky.y_noise
     * 0.000000000000000e+00 2.094757619176503e+02
     * 1.000000000000000e+00 2.094757619176503e+02
     * 2.000000000000000e+00 2.094757619176503e+02
     */
    std::unique_ptr<vv::Matrix> readCvleMatrix(std::ifstream& outFile,
            std::string& line, std::vector <std::string>& tokens,
            unsigned int nb_rows);

    std::unique_ptr<value::Map> run(wrapper_init& init, manager::Error& err);

    std::unique_ptr<vpz::Vpz> init_embedded_model(const wrapper_init& init,
            manager::Error& err);

    std::unique_ptr<value::Map> init_results();

    std::unique_ptr<value::Map> run_with_threads(
            const wrapper_init& init, manager::Error& err);

    std::unique_ptr<value::Map> run_with_cvle(
                const wrapper_init& init, manager::Error& err);

    //setup the model
    void post_define(vpz::Vpz& model);
    void post_propagates(vpz::Vpz& model, const wrapper_init& init);

    void post_inputs(vpz::Vpz& model, const wrapper_init& init);

    void clear();
};

}}//namespaces

#endif
