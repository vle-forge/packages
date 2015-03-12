/*
 * @file vle/extension/mas/Message.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2013-2015 INRA http://www.inra.fr
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
#ifndef MESSAGE_HPP
#define MESSAGE_HPP
#include <vle/value/Value.hpp>
#include <unordered_map>
#include <vle/extension/mas/PropertyContainer.hpp>

namespace vle {
namespace extension {
namespace mas {

namespace vv = vle::value;

/** @class Message
 *  @brief Allows to handle messages received from the network, or to send a
 *         message
 *
 * This class uses generic type vle::value:Value to store informations. You can
 * easily send a message by specifing a sender in constructor.
 * The Message::BROADCAST value allows to end a message to all agents.
 *
 * @see vle::value:Value
 */
class Message : public PropertyContainer
{
/* Public functions */
public:
    Message(const std::string&,const std::string&,const std::string&);

    inline std::string getSender() const
    {return mSender;}

    inline std::string getReceiver() const
    {return mReceiver;}

    inline std::string getSubject() const
    {return mSubject;}

/* Private functions */
private:
    Message();

/* Public constants */
public:
    static const std::string BROADCAST;

/* Private members */
private:
    std::string mSender;
    std::string mReceiver;
    std::string mSubject;
};

}}} //namespace vle extension mas
#endif
