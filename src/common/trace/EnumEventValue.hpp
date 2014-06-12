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
#ifndef _TIBEE_COMMON_ENUMEVENTVALUE_HPP
#define _TIBEE_COMMON_ENUMEVENTVALUE_HPP

#include <string>
#include <babeltrace/ctf/events.h>

#include <common/trace/AbstractEventValue.hpp>
#include <common/trace/EventValueType.hpp>

namespace tibee
{
namespace common
{

/**
 * Event value carrying an enumeration item value.
 *
 * @author Philippe Proulx
 */
class EnumEventValue :
    public AbstractEventValue
{
public:
    /**
     * Builds an enumeration item value out of a field definition.
     *
     * @param def          BT field definition
     * @param valueFactory Value factory used to create other event values
     */
    EnumEventValue(const ::bt_definition* def,
                   const EventValueFactory* valueFactory);

    /**
     * Returns the integer value of this enumeration item.
     *
     * @returns Enumeration item integer value
     */
    std::uint64_t getIntValue() const;

    /**
     * Returns the in-place string value of this enumeration item
     * (must be copied by user).
     *
     * @returns In-place string value of the enumeration item
     */
    const char* getLabel() const;

    /**
     * Returns a copy of the enumeration item string value.
     *
     * @returns Enumeration item string value copy
     */
    std::string getLabelStr() const;

private:
    std::string toStringImpl() const;

private:
    const ::bt_definition* _btDef;
};

}
}

#endif // _TIBEE_COMMON_ENUMEVENTVALUE_HPP
