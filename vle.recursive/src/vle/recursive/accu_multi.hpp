 /*
 * Copyright (C) 2009-2010 INRA
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
#ifndef VLE_RECURSIVE_ACCU_MULTI_HPP_
#define VLE_RECURSIVE_ACCU_MULTI_HPP_

#include <algorithm>
#include <vector>
#include <math.h>
#include <functional>
#include <numeric>
#include <limits>

#include <vle/utils/Exception.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/recursive/accu_mono.hpp>

namespace vu = vle::utils;

namespace vle { namespace recursive {

/**
 * @brief Instantiation of multi dimensional accumulator STORE
 */

class AccuMulti
{
public:
    /**
     * @brief Accu constructor
     * @param  args, common accumulator initialization structure
     */
    AccuMulti(AccuType type) : accu(type),
        init_size(false), mstats()
    {
    }

    ~AccuMulti()
    {}

    inline bool hasSize()
    {
        return init_size;
    }

    inline void setSize(unsigned int s)
    {
        mstats.clear();
        for (unsigned int i=0; i<s; i++){
            mstats.push_back(AccuMono(accu));
        }
    }

    inline unsigned int size()
    {
        return mstats.size();
    }

    void insert(const vle::value::ConstVectorView& col)
    {
        unsigned int nbEl = col.size();
        bool hasName = not col[0]->isDouble();
        if (hasName) {//first element is the col name
            nbEl --;
        }
        if (not hasSize()) {
            setSize(nbEl);
        } else {
            if (nbEl != mstats.size()) {
                throw "error";
            }
        }
        for (unsigned int i=0; i < nbEl ; i++) {
            if (hasName) {
                mstats[i].insert(col[i+1]->toDouble().value());
            } else {
                mstats[i].insert(col[i]->toDouble().value());
            }
        }
    }

    /**
     * @brief Mean vector extractor
     * @param the mean values
     */
    void mean(vle::value::Tuple& res)
    {
        res.value().resize(mstats.size());
        for (unsigned int i = 0; i < res.size(); i++) {
            res[i] = mstats[i].mean();
        }
    }
    /**
     * @brief Quantile vector extractor
     * @param the vector of results
     * @param quantileOrder the quantile order
     */
    void quantile(vle::value::Tuple& res, double quantileOrder)
    {
        res.value().resize(mstats.size());
        for (unsigned int i = 0; i < res.size(); i++) {
            res[i] = mstats[i].quantile(quantileOrder);
        }
    }

    /**
     * @brief Count extractor
     * @return the number of MultiDim objects accumulated
     */
    inline unsigned int count()
    {
        return mstats[0].count();
    }

private:
    AccuType accu;
    bool init_size;
    std::vector<AccuMono > mstats;
};

}} //namespaces

#endif

