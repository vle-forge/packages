/*
 * @file vle/oov/plugins/Console.cpp
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


#include <vle/oov/Plugin.hpp>
#include <vle/utils/DateTime.hpp>
#include <vle/utils/Path.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Map.hpp>
#include <iostream>

namespace vle { namespace oov { namespace plugin {

class Console : public Plugin
{
    /** Define a dictionary (model's name & port, index) */
    typedef std::map < std::string, int > Columns;

    /** Define the buffer of values. */
    typedef std::vector < value::Value* > Line;

    /** Define the buffer for valid values (model observed). */
    typedef std::vector < bool > ValidElement;

    /** Define a new bag indicator*/
    typedef std::map < std::string, double> NewBagWatcher;

    bool mFlushByBag;
    bool mJulian;
    bool mIsStart;
    double mTime;
    bool mHaveFirstEvent;
    bool mHeader;
    Columns mColumns;
    Line mBuffer;
    ValidElement mValid;
    NewBagWatcher mNewBagWatcher;

public:
    Console(const std::string& location)
        : Plugin(location), mFlushByBag(false), mJulian(false),
          mIsStart(false), mTime(-1.0), mHaveFirstEvent(false),
          mHeader(true)
    {}

    virtual ~Console()
    {}

    virtual void onParameter(const std::string& /* plugin */,
                             const std::string& /* location */,
                             const std::string& /* file */,
                             value::Value* parameters,
                             const double& /*time*/)
    {
        if (parameters and parameters->isMap()) {
            const value::Map& map = parameters->toMap();
            if (map.exist("locale")) {
                std::string locale(map.getString("locale"));
                try {
                    if (locale == "user") {
                        std::locale selected("");
                        std::cout.imbue(selected);
                    } else {
                        std::locale selected(locale.c_str());
                        std::cout.imbue(selected);
                    }
                } catch (...) {
                    std::locale selected("C");
                    std::cout.imbue(selected);
                }
            }

            if (map.exist("flush-by-bag")) {
                mFlushByBag = map.getBoolean("flush-by-bag");
            }

            if (map.exist("julian-day")) {
                mJulian = map.getBoolean("julian-day");
            }

            if (map.exist("header")) {
                mHeader = map.getBoolean("header");
            }
        }
        delete parameters;
    }

    void onNewObservable(const std::string& simulator,
                         const std::string& parent,
                         const std::string& portname,
                         const std::string& /* view */,
                         const double& time)
    {
        if (mIsStart) {
            flush();
        } else {
            if (not mHaveFirstEvent) {
                mTime = time;
                mHaveFirstEvent = true;
            } else {
                flush();
                mIsStart = true;
            }
        }

        std::string name(buildname(parent, simulator, portname));

        if (mColumns.find(name) != mColumns.end()) {
            throw utils::InternalError(
                fmt(_("Output plug-in: observable '%1%' already exist")) %
                name);
        }

        mNewBagWatcher[name] = -1.0;
        mColumns[name] = mBuffer.size();
        mBuffer.push_back((value::Value*)0);
        mValid.push_back(false);
    }

    void onDelObservable(const std::string& /* simulator */,
                         const std::string& /* parent */,
                         const std::string& /* portname */,
                         const std::string& /* view */,
                         const double& /* time */)
    {
    }

    void onValue(const std::string& simulator,
                 const std::string& parent,
                 const std::string& port,
                 const std::string& /* view */,
                 const double& time,
                 value::Value* value)
    {
        std::string name(buildname(parent, simulator, port));
        Columns::iterator it;

        if (not simulator.empty()) {
            name = buildname(parent, simulator, port);
            it = mColumns.find(name);

            if (it == mColumns.end()) {
                throw utils::InternalError(
                    fmt(
                        _("Output plugin: columns '%1%' does not exist. "
                          "No observable ?")) % name);
            }

            if (mIsStart) {
                if (time != mTime ||
                    (mFlushByBag &&
                     mNewBagWatcher[name] == time)) {
                    flush();
                }
            } else {
                if (not mHaveFirstEvent) {
                    mHaveFirstEvent = true;
                } else {
                    flush();
                    mIsStart = true;
                }
            }

            mBuffer[it->second] = value;
            mValid[it->second] = true;

            mNewBagWatcher[name] = time;
        }
        mTime = time;
    }

    void close(const double& time)
    {
        finalFlush(time);

        if (mHeader) {
            std::vector < std::string > array(mColumns.size());

            {
                Columns::iterator it = mColumns.begin();
                for (it = mColumns.begin(); it != mColumns.end(); ++it) {
                    array[it->second] = it->first;
                }
            }

            std::cout << "time";

            if (not array.empty()) {
                std::vector < std::string >::iterator it = array.begin();
                while (it != array.end()) {
                    std::cout << *it;
                    ++it;

                    if (it != array.end()) {
                        std::cout << "\t";
                    }
                }
            }

            std::cout << "\n";
        }
    }

    void flush()
    {
        if (mValid.empty() or std::find(mValid.begin(), mValid.end(),
                                        true) != mValid.end()) {
            std::cout << mTime;
            if (mJulian) {
                std::cout << '\t';
                try {
                    std::cout << utils::DateTime::toJulianDay(mTime);
                } catch (const std::exception& /*e*/) {
                    throw utils::ModellingError(
                        _("Output plug-in: Year is out of valid range "
                          "in julian day: 1400..10000"));
                }
            }

            std::cout << '\t';

            const size_t nb(mBuffer.size());
            for (size_t i = 0; i < nb; ++i) {
                if (mBuffer[i]) {
                    mBuffer[i]->writeFile(std::cout);
                } else {
                    std::cout << "NA";
                }

                if (i + 1 < nb) {
                    std::cout << '\t';
                }
                delete mBuffer[i];
                mBuffer[i] = 0;
                mValid[i] = false;
            }
            std::cout << "\n";
        }
    }

    void finalFlush(double trameTime)
    {
        flush();

        if (mValid.empty() or std::find(mValid.begin(), mValid.end(), true) !=
            mValid.end()) {
            std::cout << trameTime << '\t';
            for (Line::iterator it = mBuffer.begin(); it != mBuffer.end();
                 ++it) {
                if (*it) {
                    (*it)->writeFile(std::cout);
                } else {
                    std::cout << "NA";
                }

                if (it + 1 != mBuffer.end()) {
                    std::cout << '\t';
                }
                delete *it;
                *it = 0;
            }
            std::cout << "\n";
        }
    }

    std::string buildname(const std::string& parent,
                          const std::string& simulator,
                          const std::string& port)
    {
        std::string r(parent);

        r += ':';
        r += simulator;
        r += '.';
        r += port;

        return r;
    }
};

}}} // namespace vle oov plugin

DECLARE_OOV_PLUGIN(vle::oov::plugin::Console)
