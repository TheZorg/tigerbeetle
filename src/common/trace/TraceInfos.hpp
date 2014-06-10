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
#ifndef _TIBEE_COMMON_TRACEINFOS_HPP
#define _TIBEE_COMMON_TRACEINFOS_HPP

#include <map>
#include <memory>
#include <boost/filesystem.hpp>

#include <common/BasicTypes.hpp>

namespace tibee
{
namespace common
{

/**
 * Informations about a specific trace.
 *
 * TraceInfos objects are immutable, built once a trace is succesfully
 * added to a trace set.
 *
 * @see TraceSet
 *
 * @author Philippe Proulx
 */
class TraceInfos
{
public:
    /// (environment name -> environment value) map
    typedef std::map<std::string, std::string> Environment;

    /// (event name -> event ID) map
    typedef std::map<std::string, event_id_t> EventMap;

public:
    /**
     * Builds trace informations.
     *
     * @param path     Path of this trace
     * @param id       Trace ID (unique within a trace set)
     * @param env      Trace environment
     * @param eventMap Map of event names to event IDs
     */
    TraceInfos(const boost::filesystem::path& path, trace_id_t id,
               std::unique_ptr<Environment> env,
               std::unique_ptr<EventMap> eventMap);

    /**
     * Returns the trace path.
     *
     * @returns Trace path
     */
    const boost::filesystem::path& getPath() const
    {
        return _path;
    }

    /**
     * Returns the trace ID.
     *
     * @returns Trace ID
     */
    trace_id_t getId() const
    {
        return _id;
    }

    /**
     * Returns the trace environment.
     *
     * @returns Trace environment
     */
    const Environment& getEnv() const
    {
        return *_env;
    }

    /**
     * Returns the trace event map.
     *
     * An event map maps event names to event IDs.
     *
     * @returns Trace event map
     */
    const EventMap& getEventMap() const
    {
        return *_eventMap;
    }

    /**
     * Returns the trace type.
     *
     * @returns Trace type
     */
    const std::string& getTraceType() const
    {
        return _traceType;
    }

private:
    boost::filesystem::path _path;
    trace_id_t _id;
    std::unique_ptr<Environment> _env;
    std::unique_ptr<EventMap> _eventMap;
    std::string _traceType;
};

}
}

#endif // _TIBEE_COMMON_TRACEINFOS_HPP
