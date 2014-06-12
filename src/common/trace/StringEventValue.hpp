/* Copyright (c) 2014 Philippe Proulx <eepp.ca>
 *
 * This file is part of tigerbeetle.
 *
 * tigerbeetle is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tigerbeetle is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tigerbeetle.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _TIBEE_COMMON_STRINGEVENTVALUE_HPP
#define _TIBEE_COMMON_STRINGEVENTVALUE_HPP

#include <string>
#include <babeltrace/ctf/events.h>

#include <common/trace/AbstractEventValue.hpp>
#include <common/trace/EventValueType.hpp>

namespace tibee
{
namespace common
{

/**
 * Event value carrying a string.
 *
 * @author Philippe Proulx
 */
class StringEventValue :
    public AbstractEventValue
{
public:
    /**
     * Builds a string value out of a field definition.
     *
     * @param def          BT field definition
     * @param valueFactory Value factory used to create other event values
     */
    StringEventValue(const ::bt_definition* def,
                     const EventValueFactory* valueFactory);

    /**
     * Returns the in-place string value (must be copied by user).
     *
     * @returns In-place string value
     */
    const char* getValue() const;

    /**
     * Returns a copy of the string value.
     *
     * @returns String value copy
     */
    std::string getValueStr() const;

private:
    std::string toStringImpl() const;

private:
    const ::bt_definition* _btDef;
};

}
}

#endif // _TIBEE_COMMON_STRINGEVENTVALUE_HPP
