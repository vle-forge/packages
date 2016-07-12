/*
 * Copyright 2016 INRA
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.  You may
 * obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.  See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <vle/oov/Plugin.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Double.hpp>
#include <vle/utils/Exception.hpp>
#include <fstream>
#include <map>
#include <limits>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>

class GridOutput : public vle::oov::Plugin
{
    double m_current_time;
    std::vector<double> m_array;
    std::string m_location;
    std::string m_file;
    int m_height;
    int m_width;
    int m_snapshot;

    std::pair<int, int> split(const std::string &parent)
    {
        auto it = parent.find('-');
        if (it == std::string::npos)
            throw vle::utils::ArgError("GridOutput: bad simulator name");

        auto simulator = parent.substr(it + 1, std::string::npos);
        it = simulator.find(".");

        if (it == 0 or it == std::string::npos or it > simulator.size() - 1)
            throw vle::utils::ArgError("GridOutput: bad simulator name");

        int xx, yy;
        try {
            xx = std::stoi(simulator.substr(0, it));
            yy = std::stoi(simulator.substr(it + 1, std::string::npos));
        } catch (const std::exception &e) {
            throw vle::utils::ArgError("GridOutput: bad simulator name");
        }

        if (xx < 0 or yy < 0)
            throw vle::utils::ArgError("GridOutput: bad simulator name");

        return {xx, yy};
    }

    void write()
    {
        std::ostringstream filename;
        filename << m_file << "-"
            << std::setfill('0') << std::setw(8) << m_snapshot++ << ".dat";

        std::cout << "try: " << filename.str().c_str() << '\n';

        std::ofstream ofs(filename.str().c_str());
        ofs << "# time: " << m_current_time << "\n";

        if (not ofs.is_open())
            throw vle::utils::ArgError("GridOutput: fail to open %s",
                                       filename.str().c_str());

        ofs.imbue(std::locale("C"));
        ofs.precision(std::numeric_limits<double>::digits10);

        for (auto i = 0, end_i = m_height; i != end_i; ++i) {
            for (auto j = 0, end_j = m_width; j != end_j; ++j) {
                ofs << m_array[i * m_width + j] << ' ';
            }
            ofs << '\n';
        }
    }

public:
    GridOutput(const std::string &location)
        : vle::oov::Plugin(location), m_height(0), m_width(0), m_snapshot(0)
    {
    }

    virtual ~GridOutput() {}

    virtual std::unique_ptr<vle::value::Matrix> matrix() const override
    {
        return {nullptr};
    }

    virtual std::string name() const override
    {
        return std::string("GridOutput");
    }

    virtual void onParameter(const std::string & /*plugin*/,
                             const std::string &location,
                             const std::string &file,
                             std::unique_ptr<vle::value::Value> /*parameters*/,
                             const double &time) override
    {
        m_current_time = time;
        m_location = location;
        m_file = file;
        m_height = 0;
        m_width = 0;
        m_snapshot = 0;
    }

    virtual void onNewObservable(const std::string & /* simulator */,
                                 const std::string & parent,
                                 const std::string & /* port */,
                                 const std::string & /* view */,
                                 const double &time) override
    {
        auto sim = split(parent);

        m_width = std::max(m_width, sim.first);
        m_height = std::max(m_height, sim.second);

        m_current_time = time;
    }

    virtual void onDelObservable(const std::string & /* simulator */,
                                 const std::string & /* parent */,
                                 const std::string & /* port */,
                                 const std::string & /* view */,
                                 const double &time) override
    {
        if (m_current_time != time)
            write();

        m_current_time = time;
    }

    virtual void onValue(const std::string &/* simulator */,
                         const std::string &parent,
                         const std::string & /* port */,
                         const std::string & /* view */,
                         const double &time,
                         std::unique_ptr<vle::value::Value> value) override
    {
        if (m_array.empty()) { // If never initialized
            m_width++;
            m_height++;
            if (m_width * m_height <= 0)
                throw vle::utils::ModellingError("GridOutput: bad height (%d)"
                                                 " or bad width (%d)", m_height,
                                                 m_width);

            m_array.resize(m_width * m_height, 0.0);
        }

        if (m_current_time != time)
            write();

        auto sim = split(parent);

        m_array[sim.second * m_width + sim.first] = vle::value::toDouble(value);
        m_current_time = time;
    }

    /**
     * Call when the simulation is finished.
     */
    virtual std::unique_ptr<vle::value::Matrix>
        finish(const double &time) override
        {
            if (m_current_time != time)
                write();

            m_current_time = time;

            std::cout << "for i in $(ls *dat) ; do\n"
                << "gnuplot <<- EOF\n"
                << "set term png\n"
                << "set output \"${i}.png\"\n"
                << "set palette defined (0 \"white\", 2 \"blue\")\n"
                << "set cbrange [0:2]\n"
                << "plot \"${i}\"  matrix with image\n"
                << "EOF\n"
                << "done\n";

            std::cout << "ffmpeg -framerate 1/1 -i " << m_file
                << "-%08d.dat.png -c:v libx264 -r 30 "
                << "-pix_fmt yuv420p out.mp4\n";

            return {};
        }
};

DECLARE_OOV_PLUGIN(GridOutput)
