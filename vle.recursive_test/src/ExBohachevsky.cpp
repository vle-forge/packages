/*
 * Copyright (C) 2009-2014 INRA
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

//@@tagdynamic@@


#include <vle/devs/Dynamics.hpp>
#include <vle/value/Double.hpp>
#include <vle/utils/Rand.hpp>


#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

namespace vle {
namespace recursive {
namespace test {

namespace vd = vle::devs;
namespace vv = vle::value;



/**
 * @brief VLE dynamics that computes the value of ExBohachevsky function
 * for two inputs. The minimal value of this function is 0 for
 * (x1,x2) = (0,0)
 */
class ExBohachevsky: public vd::Dynamics
{

public:
    ExBohachevsky(const vd::DynamicsInit& init, const vd::InitEventList& events) :
        vd::Dynamics(init,events)
    {
        double x1 = events.getDouble("x1");
        double x2 = events.getDouble("x2");

        my = pow(x1,2) + 2 * pow(x2,2) - 0.3 * cos(3 * M_PI * x1)
                - 0.4 * cos(4 * M_PI * x2) + 0.7;

        mrand.seed(events.getInt("seed"));
        double noise = mrand.normal(0,0.1);
        my_noise = my + noise;
    }

    virtual ~ExBohachevsky()
    {
    }
    vd::Time init(vd::Time /* time */) override
    {
        return vd::infinity;
    }
    std::unique_ptr<value::Value> observation(
            const vd::ObservationEvent& event) const override
    {
        if (event.onPort("y")) {

                return value::Double::create(my);
        }
        if (event.onPort("y_noise")) {
            return value::Double::create(my_noise);
        }
        return nullptr;
    }
private:
    /**
     * @brief Output of the ExBohachevsky function
     */
    double my;
    double my_noise;
    vle::utils::Rand mrand;

};

}}}// namespaces

DECLARE_DYNAMICS(vle::recursive::test::ExBohachevsky)
