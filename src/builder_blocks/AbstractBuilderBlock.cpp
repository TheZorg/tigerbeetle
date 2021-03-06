/* Copyright (c) 2014 Francois Doray <francois.pierre-doray@polymtl.ca>
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
#include "builder_blocks/AbstractBuilderBlock.hpp"

#include "base/Constants.hpp"
#include "block/ServiceList.hpp"

namespace tibee {
namespace builder_blocks {

AbstractBuilderBlock::AbstractBuilderBlock()
{
}

void AbstractBuilderBlock::LoadServices(const block::ServiceList& serviceList)
{
    serviceList.QueryService(kExecutionBuilderServiceName,
                             reinterpret_cast<void**>(&_executionBuilder));

    serviceList.QueryService(kCurrentStateServiceName,
                             reinterpret_cast<void**>(&_currentState));

    // Get constant quarks.
    Q_NODE_TYPE = _currentState->Quark(kNodeType);
}

uint32_t AbstractBuilderBlock::ThreadForEvent(const trace::EventValue& event) const
{
    auto cpu = event.getStreamPacketContext()->GetField("cpu_id")->AsUInteger();
    auto thread = State()->CurrentThreadForCpu(cpu);
    return thread;
}

}  // namespace builder_blocks
}  // namespace tibee
