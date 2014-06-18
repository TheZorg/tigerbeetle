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
#ifndef _TIBEE_COMMON_SINT32STATEVALUE_HPP
#define _TIBEE_COMMON_SINT32STATEVALUE_HPP

#include <memory>
#include <cstdint>

#include <common/state/SimpleStateValue.hpp>
#include <common/state/StateValueType.hpp>

namespace tibee
{
namespace common
{

/**
 * 32-bit signed integer state value.
 *
 * @author Philippe Proulx
 */
class Sint32StateValue :
    public SimpleStateValue<std::int32_t, StateValueType::SINT32>
{
public:
    typedef std::shared_ptr<Sint32StateValue> SP;
    typedef std::unique_ptr<Sint32StateValue> UP;

public:
    using SimpleStateValue::SimpleStateValue;
};

}
}

#endif // _TIBEE_COMMON_SINT32STATEVALUE_HPP
