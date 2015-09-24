/*
 * @file vle/oov/plugins/Storage.hpp
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
#include <vle/value/Double.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Map.hpp>
#include <string>
#include <vector>
#include <map>
#include <list>

namespace vle { namespace oov { namespace plugin {

static std::string buildKey(const std::string& parent,
                            const std::string& simulator,
                            const std::string& port)
{
    std::string result;

    result.reserve(parent.size() + simulator.size() + port.size() + 3);

    result = parent;
    result += ':';
    result += simulator;
    result += '.';
    result += port;

    return result;
}

typedef value::MatrixValue::index Index;

typedef std::map < std::string, Index > MapPairIndex;

enum StorageHeaderType
{
    STORAGE_HEADER_NONE,        /**< No header are provided, ie. the
                                 * names of the simulators are hidden  */
    STORAGE_HEADER_TOP          /**< Header (the names of the colums
                                 * are stored into the first line of
                                 * the matrix results. */
};

class Storage : public Plugin
{
public:
    Storage(const std::string& location)
        : Plugin(location),
          m_matrix(new value::Matrix(1, 0, 10, 10, 10, 10)),
          m_time(-HUGE_VAL),
          m_isstart(false),
          m_headertype(STORAGE_HEADER_NONE)
    {
    }

    /**
     * We do not remove the m_matrix attribute. The @c
     * manager::Simulation can retrieve it from the @c
     * devs::RootCoordinator. Users have in charge to freed the
     * m_matrix.
     */
    virtual ~Storage()
    {
    }

    /**
     * @attention We do not remove the m_matrix attribute. The @c
     * manager::Simulation can retrieve it from the @c
     * devs::RootCoordinator. Users have in charge to freed the
     * m_matrix.
     */
    virtual value::Matrix * matrix() const
    {
        return m_matrix;
    }

    virtual std::string name() const
    {
        return std::string("storage");
    }

    ///
    ////
    ///

    virtual void onParameter(const std::string& /*plugin*/,
                             const std::string& /*location*/,
                             const std::string& /*file*/,
                             value::Value* parameters,
                             const double& /*time*/)
    {
        if (parameters) {
            if (not parameters->isMap()) {
                throw utils::ArgError(
                    _("Storage: initialization failed, bad parameters"));
            }
            value::Map* init = dynamic_cast < value::Map* >(parameters);
            int columns = -1, rows = -1, rzcolumns = -1, rzrows = -1;

            if (init->exist("columns")) {
                columns = value::toInteger(init->get("columns"));
            }

            if (init->exist("rows")) {
                rows = value::toInteger(init->get("rows"));
            }

            if (init->exist("inc_columns")) {
                rzcolumns = value::toInteger(init->get("inc_columns"));
            }

            if (init->exist("inc_rows")) {
                rzrows = value::toInteger(init->get("inc_rows"));
            }

            if (init->exist("header")) {
                std::string type = value::toString(init->get("header"));

                if (type == "top") {
                    m_headertype = STORAGE_HEADER_TOP;
                }
            }
            if (columns > 0 && rows > 0) {
                m_matrix->resize(columns, rows);
            }
            m_matrix->setResizeColumn(rzcolumns > 0 ? rzcolumns : 10);
            m_matrix->setResizeRow(rzrows > 0 ? rzrows : 10);

            delete parameters;
        }

        if (m_headertype == STORAGE_HEADER_TOP) {
            m_matrix->addRow();
            m_matrix->add(0, 0, new vle::value::String("time"));
        }
    }

    virtual void onNewObservable(const std::string& simulator,
                                 const std::string& parent,
                                 const std::string& port,
                                 const std::string& /*view*/,
                                 const double& /*time*/)
    {
        std::string key = buildKey(parent, simulator, port);
        Index idx = m_matrix->columns();

        std::pair < MapPairIndex::iterator, bool > r =
            m_colAccess.insert(std::make_pair(key, m_matrix->columns()));

        assert(r.second);

        m_matrix->addColumn();

        if (m_headertype == STORAGE_HEADER_TOP) {
            m_matrix->add(idx, 0, new vle::value::String(key));
        }
    }

    virtual void onDelObservable(const std::string& /*simulator*/,
                                 const std::string& /*parent*/,
                                 const std::string& /*port*/,
                                 const std::string& /*view*/,
                                 const double& /*time*/)
    {
    }

    virtual void onValue(const std::string& simulator,
                         const std::string& parent,
                         const std::string& port,
                         const std::string& /*view*/,
                         const double& time,
                         value::Value* value)
    {
        if (m_isstart) {
            nextTime(time);
        } else {
            if (m_time == -HUGE_VAL) {
                m_time = time;
            } else {
                nextTime(time);
                m_isstart = true;
            }
        }

        if (not simulator.empty()) {
            std::string key = buildKey(parent, simulator, port);

            MapPairIndex::const_iterator it = m_colAccess.find(key);
            assert(it != m_colAccess.end());

            m_matrix->add(it->second, m_matrix->rows(), value);
        }
    }

    virtual void close(const double& /*time*/)
    {
        setLastTime(m_time);
    }

private:
    value::Matrix     *m_matrix;
    MapPairIndex       m_colAccess;
    double             m_time;
    bool               m_isstart;
    StorageHeaderType  m_headertype;

    inline void nextTime(double trame_time)
    {
        if (trame_time != m_time) {
            setLastTime(m_time);
            m_time = trame_time;
        }
    }

    inline void setLastTime(double time)
    {
        value::Matrix::size_type row(m_matrix->rows());
        m_matrix->addRow();
        m_matrix->add(0, row, new vle::value::Double(time));
    }
};

}}} // namespace vle oov plugin

DECLARE_OOV_PLUGIN(vle::oov::plugin::Storage)
