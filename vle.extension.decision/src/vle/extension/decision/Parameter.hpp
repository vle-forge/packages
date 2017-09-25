/* Copyright (C) 2017 INRA
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef ORG_VLEPROJECT_BARYONYX_SOLVER_CORE
#define ORG_VLEPROJECT_BARYONYX_SOLVER_CORE

#include <algorithm>
#include <chrono>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <cstdarg>
#include <cstdio>

#if defined _WIN32 || defined __CYGWIN__
#define BARYONYX_HELPER_DLL_IMPORT __declspec(dllimport)
#define BARYONYX_HELPER_DLL_EXPORT __declspec(dllexport)
#define BARYONYX_HELPER_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define BARYONYX_HELPER_DLL_IMPORT __attribute__((visibility("default")))
#define BARYONYX_HELPER_DLL_EXPORT __attribute__((visibility("default")))
#define BARYONYX_HELPER_DLL_LOCAL __attribute__((visibility("hidden")))
#else
#define BARYONYX_HELPER_DLL_IMPORT
#define BARYONYX_HELPER_DLL_EXPORT
#define BARYONYX_HELPER_DLL_LOCAL
#endif
#endif

#ifdef BARYONYX_DLL
#ifdef libbaryonyx_EXPORTS
#define BARYONYX_API BARYONYX_HELPER_DLL_EXPORT
#else
#define BARYONYX_API BARYONYX_HELPER_DLL_IMPORT
#endif
#define BARYONYX_LOCAL BARYONYX_HELPER_DLL_LOCAL
#define BARYONYX_MODULE BARYONYX_HELPER_DLL_EXPORT
#else
#define BARYONYX_API
#define BARYONYX_LOCAL
#define BARYONYX_MODULE BARYONYX_HELPER_DLL_EXPORT
#endif

#if defined(__GNUC__)
#define BARYONYX_FORMAT(idformat, idarg)                                      \
    __attribute__((format(printf, (idformat), (idarg))))
#else
#define BARYONYX_FORMAT(idformat, idarg)
#endif

namespace baryonyx {

/**
 * @brief @c baryonyx::paramter is a simple class to represent a union
 *     between an integer, a real and a string (resp. @c long, @c double
 *     and @c std::string).
 *
 * @details Waiting for a compiler with the @c std::optional class, @c
 *     baryonyx::parameter implements an union between a @c long, a @c
 *     double and a @c std::string. @c baryonyx::parameter, by default,
 *     defines a @c long with the value of @c 0. Use the @c
 *     baryonyx::parameter::type function to get the current type stored
 *     into the @c baryonyx::parameter.
 */
class parameter
{
public:
    enum class tag
    {
        integer,
        real,
        string
    };

    parameter()
      : l(0)
      , type(tag::integer)
    {
    }

    explicit parameter(long value_)
      : l(value_)
      , type(tag::integer)
    {
    }

    explicit parameter(double value_)
      : d(value_)
      , type(tag::real)
    {
    }

    explicit parameter(const std::string& value_)
      : s(value_)
      , type(tag::string)
    {
    }

    parameter(const parameter& w)
      : type(w.type)
    {
        switch (w.type) {
        case tag::integer:
            l = w.l;
            break;
        case tag::real:
            d = w.d;
            break;
        case tag::string:
            new (&s) std::string(w.s);
            break;
        }
    }

    parameter(parameter&& w)
      : type(w.type)
    {
        switch (w.type) {
        case tag::integer:
            l = w.l;
            break;
        case tag::real:
            d = w.d;
            break;
        case tag::string:
            new (&s) std::string(w.s);
            break;
        }

        w.type = tag::integer;
        w.l = 0;
    }

    parameter& operator=(const parameter& w)
    {
        if (type == tag::string and w.type == tag::string) {
            s = w.s;
            return *this;
        }

        if (type == tag::string) {
            using std::string;
            s.~string();
        }

        switch (w.type) {
        case tag::integer:
            l = w.l;
            break;
        case tag::real:
            d = w.d;
            break;
        case tag::string:
            new (&s) std::string(w.s);
            break;
        }

        type = w.type;
        return *this;
    }

    parameter& operator=(parameter&& w)
    {
        if (type == tag::string and w.type == tag::string) {
            new (&s) std::string(w.s);
            w.type = tag::integer;
            w.l = 0;
            return *this;
        }

        if (type == tag::string) {
            using std::string;
            s.~string();
        }

        switch (w.type) {
        case tag::integer:
            l = w.l;
            break;
        case tag::real:
            d = w.d;
            break;
        case tag::string:
            new (&s) std::string(w.s);
            break;
        }

        type = w.type;

        w.type = tag::integer;
        w.l = 0;

        return *this;
    }

    parameter& operator=(double value)
    {
        if (type == tag::real) {
            d = value;
            return *this;
        }

        if (type == tag::string) {
            using std::string;
            s.~string();
        }

        type = tag::real;
        d = value;

        return *this;
    }

    parameter& operator=(long value)
    {
        if (type == tag::integer) {
            l = value;
            return *this;
        }

        if (type == tag::string) {
            using std::string;
            s.~string();
        }

        type = tag::integer;
        l = value;

        return *this;
    }

    parameter& operator=(const std::string& value)
    {
        if (type == tag::string) {
            s = value;
            return *this;
        }

        if (type == tag::string) {
            using std::string;
            s.~string();
        }

        type = tag::string;
        new (&s) std::string(value);

        return *this;
    }

    ~parameter() noexcept
    {
        if (type == tag::string) {
            using std::string;
            s.~string();
        }
    }

    void swap(parameter& p)
    {
        parameter copy(*this);
        *this = p;
        p = copy;
    }

    union
    {
        long l;
        double d;
        std::string s;
    };

    tag type;
};
}

#endif
