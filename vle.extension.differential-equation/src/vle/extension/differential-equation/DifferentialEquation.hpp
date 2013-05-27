/*
 * @file vle/extension/differential_equation/DifferentialEquation.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2011 INRA http://www.inra.fr
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

#ifndef VLE_EXTENSION_DIFFERENTIAL_EQUATION_DIFFERENTIAL_EQUATION_HPP
#define VLE_EXTENSION_DIFFERENTIAL_EQUATION_DIFFERENTIAL_EQUATION_HPP 1

#include <map>
#include <set>

#include <vle/devs/Dynamics.hpp>
#include <vle/utils/Exception.hpp>

namespace vle {
namespace extension {
namespace differential_equation {

namespace vd = vle::devs;
namespace vv = vle::value;
namespace vu = vle::utils;

class DifferentialEquationImpl;

class Variable
{
public:

    Variable() :
            name(""), value(0), gradient(0)
    {
    }

    Variable(const std::string& n) :
            name(n), value(0), gradient(0)
    {
    }

    inline void setGrad(double g)
    {
        gradient = g;
    }

    inline double getVal() const
    {
        return value;
    }

    inline void setVal(double v)
    {
        value = v;
    }

    inline double getGrad() const
    {
        return gradient;
    }

    inline const std::string& getName() const
    {
        return name;
    }

private:
    std::string name;
    double value;
    double gradient;
};
class ExternVariable
{
public:

    ExternVariable() :
            name(""), value(0)
    {
    }

    ExternVariable(const std::string& n) :
            name(n), value(0)
    {
    }

    inline double getVal() const
    {
        return value;
    }

    inline const std::string& getName() const
    {
        return name;
    }

    inline void set(double v)
    {
        value = v;
    }

private:
    std::string name;
    double value;
};

template<class VarType>
struct VarsContainer
{
    typedef typename std::map<std::string, VarType> Container;
    typedef typename Container::const_iterator const_iterator;
    typedef typename Container::iterator iterator;
    Container cont;

    VarsContainer() :
            cont()
    {
    }

    const_iterator begin() const
    {
        return cont.begin();
    }

    const_iterator end() const
    {
        return cont.end();
    }

    iterator begin()
    {
        return cont.begin();
    }

    iterator end()
    {
        return cont.end();
    }

    iterator find(const std::string& n)
    {
        return cont.find(n);
    }

    const_iterator find(const std::string& n) const
    {
        return cont.find(n);
    }

    unsigned int size() const
    {
        return cont.size();
    }

    VarType& operator[](const std::string& n)
    {
        return cont[n];
    }

    void operator=(const VarsContainer<VarType>& c)
    {
        const_iterator itb = c.begin();
        const_iterator ite = c.end();
        for (; itb != ite; itb++) {
            cont[itb->first] = itb->second;
        }
    }

    void add(const std::string& name)
    {
        cont.insert(std::make_pair(name, VarType(name)));
    }
};

typedef VarsContainer<Variable> Variables;
typedef VarsContainer<ExternVariable> ExternVariables;

class DifferentialEquation : public vle::devs::Dynamics
{
public:
    class Var
    {
    private:
        std::string name;
        Variable* itVar;
        DifferentialEquation* equation;

    public:

        Var() :
                name(""), itVar(0), equation(0)
        {
        }

        Var(const std::string& n, DifferentialEquation* eq) :
                name(n), itVar(0), equation(eq)
        {
        }

        inline void initPointer()
        {
            if (not itVar) {
                itVar = &(equation->vars().find(name)->second);
            }
        }

        inline void setGrad(double g)
        {
            initPointer();
            itVar->setGrad(g);
        }

        inline double getGrad()
        {
            initPointer();
            return itVar->getGrad();
        }

        inline double operator()()
        {
            initPointer();
            return itVar->getVal();
        }
    };

    class Ext
    {
    private:
        std::string name;
        ExternVariable* itExt;
        DifferentialEquation* equation;

    public:

        Ext() :
                name(""), itExt(0), equation(0)
        {
        }

        Ext(const std::string& n, DifferentialEquation* eq) :
                name(n), itExt(0), equation(eq)
        {
        }

        inline void initPointer()
        {
            if (not itExt) {
                itExt = &(equation->extVars().find(name)->second);
            }
        }

        inline double operator()()
        {
            initPointer();
            return itExt->getVal();
        }
    };

public:

    class gradStructVariable
    {
    private:
        Var& var;

    public:

        gradStructVariable(Var& v) :
                var(v)
        {
        }

        inline double operator()()
        {
            return var.getGrad();
        }

        inline void operator=(double g)
        {
            var.setGrad(g);
        }
    };

    class gradStructExternVariable
    {
    private:
        Ext& ext;

    public:

        gradStructExternVariable(Ext& v) :
                ext(v)
        {
        }
    };

//definition of functions
public:

    Var createVar(const std::string& name);

    Ext createExt(const std::string& name);

    inline gradStructVariable grad(Var& v)
    {
        return gradStructVariable(v);
    }

    inline gradStructExternVariable grad(Ext& v)
    {
        return gradStructExternVariable(v);
    }

    inline const Variables& vars() const
    {
        return mvars;
    }

    inline Variables& vars()
    {
        return mvars;
    }

    inline const ExternVariables& extVars() const
    {
        return mextVars;
    }

    inline ExternVariables& extVars()
    {
        return mextVars;
    }

    /**
     * @brief Computation of gradients, which is defined by the
     * user
     * @param time, the current time
     */
    virtual void compute(const vle::devs::Time& time) = 0;

    /**
     * @brief Implementation of the default behavior of reinit function.
     * If reinitialisation is required on perturbation,
     * evt is expected to be a map ("name","value") and
     * has the semantic of reseting the state variable to the value.
     * @param evt, the evt of perturbation
     * @param perturb, true if it is a perturbation, false if it is
     * the propagation of a discontinuity
     * @param t, the time of perturbation
     */
    virtual void reinit(const vv::Set& evt, bool perturb, const vd::Time& t);

protected:

    DifferentialEquation(const vd::DynamicsInit& model,
            const vd::InitEventList& events);

    virtual ~DifferentialEquation();

    /************** DEVS functions *****************/
    vd::Time init(const vd::Time& time);

    void output(const vd::Time& time, vd::ExternalEventList& output) const;

    vd::Time timeAdvance() const;

    void confluentTransitions(const vd::Time& time,
            const vd::ExternalEventList& extEventlist);

    void internalTransition(const vd::Time& event);

    void externalTransition(const vd::ExternalEventList& event,
            const vd::Time& time);

    vv::Value* observation(const vd::ObservationEvent& event) const;

    Variables mvars;
    ExternVariables mextVars;
    DifferentialEquationImpl* meqImpl;
    bool mdeclarationOn;
    vv::Map* minitVariables;
    vv::Map* mmethParams;
    std::string mmethod;

    friend std::ostream& operator<<(std::ostream& o, const Variable& v);

    friend std::ostream& operator<<(std::ostream& o, const ExternVariables& v);

    friend std::ostream& operator<<(std::ostream& o, const ExternVariable& v);

    friend std::ostream& operator<<(std::ostream& o, const Variables& v);

};

/**** Implementation of operator<< ****/

inline std::ostream& operator<<(std::ostream& o, const Variable& v)
{
    o << "[" << v.getName() << "=" << v.getVal() << "; g=" << v.getGrad()
            << "]";
    return o;
}

inline std::ostream& operator<<(std::ostream& o, const ExternVariable& v)
{
    o << v.getName() << "=" << v.getVal();
    return o;
}

inline std::ostream& operator<<(std::ostream& o, const Variables& vs)
{
    Variables::const_iterator itb = vs.begin();
    Variables::const_iterator ite = vs.end();
    for (; itb != ite; itb++) {
        const Variable& v = itb->second;
        o << "(" << v << "), ";
    }
    return o;

}

inline std::ostream& operator<<(std::ostream& o, const ExternVariables& vs)
{
    ExternVariables::const_iterator itb = vs.begin();
    ExternVariables::const_iterator ite = vs.end();

    for (; itb != ite; itb++) {
        const ExternVariable& v = itb->second;
        o << "(" << v << "), ";
    }
    return o;
}

}
}
} // namespace vle extension differential_equation

#endif
