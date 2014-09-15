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

#include <sstream>

#include <common/ex/TraceSet.hpp>
#include <common/trace/CreateTraceSet.hpp>

namespace tibee
{
namespace common
{

std::unique_ptr<TraceSet> CreateTraceSet(
    const std::vector<boost::filesystem::path>& tracePaths)
{
    // create a trace set
    std::unique_ptr<common::TraceSet> traceSet {new common::TraceSet};

    // add traces to trace set
    for (const auto& tracePath : tracePaths) {
        if (!traceSet->addTrace(tracePath)) {
            std::stringstream ss;

            ss << "could not add trace " << tracePath << " (internal error)";

            throw ex::TraceSet {ss.str()};
        }
    }

    return traceSet;
}

}
}
