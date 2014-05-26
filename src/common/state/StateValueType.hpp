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
#ifndef _STATEVALUETYPE_HPP
#define _STATEVALUETYPE_HPP

namespace tibee
{
namespace common
{

/**
 * State value types.
 *
 * @author Philippe Proulx
 */
enum class StateValueType
{
    INT32,
    UINT32,
    INT64,
    UINT64,
    FLOAT32,
    QUARK,
};

}
}

#endif // _STATEVALUETYPE_HPP