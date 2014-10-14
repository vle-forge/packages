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


#include "EmbeddedSimulatorMPI.hpp"

#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Spawn.hpp>
#include <vle/utils/Path.hpp>
#include <vle/manager/Manager.hpp>
#include <vle/value/Double.hpp>

#include <vle/reader/vle_results_text_reader.hpp>
#include <vle/recursive/VleAPIfacilities.hpp>
#include <vle/recursive/EmbeddedSimulator.hpp>
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

EmbeddedSimulatorMPI::EmbeddedSimulatorMPI() :
                EmbeddedSimulator(), mnbthreads(0), mnbSimus(0)
{
}

void EmbeddedSimulatorMPI::init(const vv::Map& initSim)
{
    EmbeddedSimulator::init(initSim);
    //handle parameters
    if((minputs.size() != 1) or (moutputs.size() != 1)){
        throw vu::ArgError(vle::fmt("[EmbeddedSimulatorMPI] : only one input and"
                "one output are implemented for now ; got '%1%' inputs "
                "and '%2%' outputs") % minputs.size() % moutputs.size());
    }
}

EmbeddedSimulatorMPI::~EmbeddedSimulatorMPI()
{
}

void EmbeddedSimulatorMPI::simulatePlan(const vv::Value& input)
{
    clearAllCondPort();
    postInputs(input);
    vu::Package pkg("vle.recursive");//TODO should be saved outside the rr package
    std::string tempvpzPath = pkg.getExpDir(vu::PKG_BINARY);
    tempvpzPath.append("/EmbeddedSimulatorMPItemp.vpz");
    mvpzDyn.write(tempvpzPath);
    vu::Spawn mspawn;
    std::string exe = vu::Path::findProgram("mvle");
    std::string workingDir="/tmp/";//TODO shopuld be parameterized
    std::vector < std::string > argv;
    argv.push_back("-P");//TODO should be simulated outside the rr package
    argv.push_back("vle.recursive");
    argv.push_back("EmbeddedSimulatorMPItemp.vpz");


    bool started = mspawn.start(exe, workingDir, argv);
    if (not started) {
        throw vu::ArgError(vle::fmt(_("Failed to start `%1%'")) % exe);
    }
    std::string message;
    bool is_success = true;;
    mspawn.wait();
    mspawn.status(&message, &is_success);
    if (! is_success) {
        throw vu::ArgError(vle::fmt(_("Error launching `%1%' : %2% "))
        % exe % message);
    }

}

void EmbeddedSimulatorMPI::setNbThreads(unsigned int nbThreads)
{
    mnbthreads = nbThreads;
}

void EmbeddedSimulatorMPI::postInputs(const vv::Value& input)
{
    vz::Conditions& conds = mvpzDyn.project().experiment().conditions();
    switch (input.getType()) {
    case vv::Value::TUPLE : {
        const vv::Tuple& tuple = input.toTuple();
        if (minputs.size() != 1) {
            throw vu::ArgError(vle::fmt("[EmbeddedSimulatorMPI] : wrong "
                    "input format, got a tuple (for only one input) and expect "
                    " '%1%' inputs") % minputs.size());
        }
        vz::Condition& cond = conds.get(minputs[0].cond_name);
        const std::string& portname = minputs[0].port_name;
        for (unsigned int i=0; i< tuple.size() ; i ++) {
            cond.addValueToPort(portname,new vv::Double(
                        tuple.at(i)));
        }
        mnbSimus = tuple.size();
        break;
    }
    case vv::Value::TABLE : {
        const vv::Table& table = input.toTable();
        if (minputs.size() != (unsigned int) table.height()) {
            throw vu::ArgError(vle::fmt("[EmbeddedSimulatorMPI] : wrong "
                    "input format, got a table with '%1%' rows and expect "
                    " '%2%' inputs") % table.height() % minputs.size());
        }
        for (unsigned int i=0; i< table.height() ; i ++) {
            vz::Condition& cond = conds.get(minputs[i].cond_name);
            const std::string& portname = minputs[i].port_name;
            for (unsigned int j=0; j< table.width() ; j ++) {
                cond.addValueToPort(portname,new vv::Double(
                        table.get(j,i)));
            }
        }
        break;
    }
    case vv::Value::SET : {
        const vv::Set& set = input.toSet();
        switch (set.get(0)->getType()) {
        case vv::Value::TUPLE: {
            if (minputs.size() != 1) {
                throw vu::ArgError(vle::fmt("[EmbeddedSimulatorMPI] : wrong "
                        "input format, got a tuple and expect data for %1% "
                        "inputs") % minputs.size());
            }
            for (unsigned int i=0; i<set.size() ; i++){
                const vv::Tuple& tuple = set.getTuple(i);
                vz::Condition& cond = conds.get(minputs[0].cond_name);
                cond.addValueToPort(minputs[0].port_name,tuple.clone());
            }
            break;
        }
        case vv::Value::SET: {
            for (unsigned int i=0; i<set.size() ; i++){
                const vv::Set& seti = set.getSet(i);
                if (minputs.size() != seti.size()) {
                    throw vu::ArgError(vle::fmt("[EmbeddedSimulatorMPI] : "
                            "wrong input format, got a set of size '%1%', "
                            "and expect a set of size '%2%'")
                    % seti.size() % minputs.size());
                }
                for (unsigned int j=0; j<minputs.size() ; j++){
                    vz::Condition& cond = conds.get(minputs[j].cond_name);
                    cond.addValueToPort(minputs[j].port_name,
                            seti.get(j)->clone());
                }
            }
            break;
        }
        default : {
            throw vu::ArgError(vle::fmt("[EmbeddedSimulatorMPI] : wrong "
                    "input format (got %1%)") % input);
            break;
        }
        }
        break;
    }
    default : {
        throw vu::ArgError(vle::fmt("[EmbeddedSimulatorMPI] : wrong set of "
                "inputs format (got %1%)") % input.getType());
        break;
    }
    }
}

unsigned int EmbeddedSimulatorMPI::nbSimus() const
{
    return mnbSimus;
}

double EmbeddedSimulatorMPI::getLastValueOfSim(unsigned int i) const
{
    const EmbeddedSimulatorOutput& output(moutputs[0]);
    std::string vleResultFilePath = "/tmp/";
    vleResultFilePath.append(mvpzDyn.project().experiment().name());
    vleResultFilePath.append("-");
    vleResultFilePath.append(boost::lexical_cast<std::string>(i));
    vleResultFilePath.append("_");
    vleResultFilePath.append(output.view_name);
    vleResultFilePath.append(".dat");
    vle::reader::VleResultsTextReader tfr(vleResultFilePath);
    vle::value::Matrix mat;
    tfr.readFile(mat);
    for (unsigned int j=0; j < mat.columns(); j++) {
        if (mat.getString(j,0) == output.port_name) {
            return mat.getDouble(j,mat.rows()-1);
        }
    }
    throw vu::ArgError(vle::fmt("[EmbeddedSimulatorMPI] : did not find output "
      " '%1%' into file '%2%'") % output.complete_name % vleResultFilePath);
    return 0;
}

void EmbeddedSimulatorMPI::fillWithLastValuesOfOutputs(
        vv::Value& tofill) const
{
    switch (tofill.getType()) {
    case vv::Value::TUPLE: {
        if ((moutputs.size() != 1) and (nbSimus() > 1)) {
            throw vu::ArgError(vle::fmt("[EmbeddedSimulatorMPI] : got a tuple "
              " and expect 1 output (got %1%)") % moutputs.size());
        }
        vv::Tuple& tofillTuple = tofill.toTuple();
        tofillTuple.value().resize(nbSimus());
        for (unsigned int i=0; i< nbSimus(); i++) {
            double res = getLastValueOfSim(i);
            tofillTuple[i] = res;
        }
        break;
    }
    default : {
            throw vle::utils::ArgError( vle::fmt("[EmbeddedSimulatorMPI] "
                    "fillWithLastValuesOfOutputs1 : '%1%'") % tofill.getType());
            break;
        }
    }
}

}}//namespaces
