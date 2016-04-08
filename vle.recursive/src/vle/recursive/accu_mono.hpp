/*
 * Copyright (C) 2015-2015 INRA
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

#ifndef VLE_RECURSIVE_ACCU_MONO_HPP_
#define VLE_RECURSIVE_ACCU_MONO_HPP_

#include <algorithm>
#include <vector>
#include <math.h>
#include <functional>
#include <numeric>
#include <limits>

#include <vle/utils/Exception.hpp>

namespace vle { namespace recursive {

namespace vu = vle::utils;
/**
 * @brief Different types of accumulators.
 */
enum AccuType
{
    STANDARD, //computes mean, std, var, min, max
    MEAN,     //compute mean only
    QUANTILE  //compute quantiles
};

class AccuMono
{

public:
    /**
     * @brief Accu constructor
     * @param  args, common accumulator initialization structure
     */
    AccuMono(AccuType type) :
        accu(type), msum(0), mcount(0), msquareSum(0),
        mmin(std::numeric_limits<double>::max()),
        mmax(std::numeric_limits<double>::min()),
        msorted(0), mvalues(0)
    {
        switch (accu) {
        case STANDARD:
        case MEAN: {
            break;
        } case QUANTILE: {
            mvalues = new std::vector<double>();
            msorted = true;
            break;
        } default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (1)");
            break;
        }
    }
    ~AccuMono()
    {
        delete mvalues;
    }
    /**
     * @brief Inserts a real into the accumulator
     * @param v the real value
     */
    inline void insert(double v)
    {
        switch (accu) {
        case STANDARD: {
            msum += v;
            mcount++;
            msquareSum += pow(v,2);
            if (v < mmin) {
                mmin = v;
            }
            break;
        } case MEAN: {
            msum += v;
            mcount++;
            break;
        } case QUANTILE: {
            mvalues->push_back(v);
            msorted = false;
            break;
        } default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (1)");
            break;
        }
    }

    /**
     * @brief Mean statistic extractor
     * @return the mean value
     */
    inline double mean() const
    {
        switch (accu) {
        case STANDARD:
        case MEAN: {
            return msum / mcount;
            break;
        } case QUANTILE: {
            double sum = std::accumulate(mvalues->begin(),mvalues->end(),0.0);
            return sum / mvalues->size();
        } default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (2)");
            return 0;
            break;
        }
    }
    /**
     * @brief Moment of order 2 extractor :
     * (sum of squares) divided by count
     * @return the moment of order 2 value
     */

    inline double moment2() const
    {
        switch (accu) {
        case STANDARD: {
            return msquareSum / mcount;
            break;
        } case MEAN: {
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        } default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (2)");
            return 0;
            break;
        }
    }
    /**
     * @brief Deviation Square sum extractor (requires rewriting) :
     * let N = count ; X = mean ; x_i the ieme value inserted.
     * sum_i [(x_i - X) ^ 2] = N * X^2 + sum_i (x_i ^ 2) - 2X sum_i(x_i)
     * @return the deviation square sum
     */
    inline double deviationSquareSum() const
    {
        switch (accu) {
        case STANDARD: {
            double mean = msum / mcount;
            return mcount * pow(mean,2) + msquareSum - 2 * mean * msum;
            break;
        } case MEAN:
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (2)");
            return 0;
            break;
        }
    }
    /**
     * @brief Standad deviation,
     * note: this is not the sample standard deviation
     * @return the stdDeviation
     */
    inline double stdDeviation() const
    {
        switch (accu) {
        case STANDARD: {
            return sqrt(deviationSquareSum()/mcount);
            break;
        } case MEAN:
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (2)");
            return 0;
            break;
        }
    }
    /**
     * @brief Square sum extractor
     * @return suqre sum value
     */
    inline double squareSum() const
    {
        switch (accu) {
        case STANDARD:
            return msquareSum;
            break;
        case MEAN:
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        case QUANTILE: {
            double squareSum = 0;
            std::vector<double>::const_iterator itb = mvalues->begin();
            std::vector<double>::const_iterator ite = mvalues->end();
            for (; itb != ite; itb++) {
                squareSum += pow(*itb,2);
            }
            return squareSum;
        } default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (2)");
            return 0;
            break;
        }
    }
    /**
     * @brief Count extractor
     * @return the count value
     */
    inline unsigned int count() const
    {
        switch (accu) {
        case STANDARD:
        case MEAN:
            return mcount;
            break;
        case QUANTILE:
            return mvalues->size();
            break;
        default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (3)");
            return 0;
            break;
        }

    }
    /**
     * @brief Sum extractor
     * @return the sum value
     */
    inline double sum() const
    {
        switch (accu) {
        case STANDARD:
        case MEAN:
            return msum;
            break;
        case QUANTILE:
            return std::accumulate(mvalues->begin(),mvalues->end(),0.0);
            break;
        default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (4)");
            return 0;
            break;
        }
    }
    /**
     * @brief Minimal value extractor
     * @return the minimal value
     */
    inline double min() const
    {
        switch (accu) {
        case STANDARD:
            return mmin;
            break;
        case MEAN:
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        case QUANTILE:
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (5)");
            return 0;
            break;
        }
    }

    /**
     * @brief Quantile value extractor (weigthed average method)
     * @param quantileOrder the quantile order
     * @returns the quantile value of order quantileOrder
     */
    inline double quantile(double quantileOrder)
    {
        switch (accu) {
        case STANDARD:
        case MEAN:
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        case QUANTILE: {
            if (!msorted) {
                std::sort(mvalues->begin(),mvalues->end());
                msorted = true;
            }
            double quantile = 0;
            if (quantileOrder == 1.0) {
                quantile = (*mvalues)[mvalues->size() - 1];
            } else {
                double rang_quantile = (mvalues->size() - 1) * quantileOrder;
                int ent_rang = floor(rang_quantile);
                double frac_rang = rang_quantile - ent_rang;
                quantile = (*mvalues)[ent_rang] + frac_rang *
                        ((*mvalues)[ent_rang + 1] - (*mvalues)[ent_rang]);
            }
            return quantile;
            break;
        } default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (6)");
            return 0;
            break;
        }
    }
protected:
    AccuType accu;
    double msum;
    unsigned int mcount;
    double msquareSum;
    double mmin;
    double mmax;
    bool msorted;
    std::vector<double>* mvalues;

};

}} //namespaces

#endif

