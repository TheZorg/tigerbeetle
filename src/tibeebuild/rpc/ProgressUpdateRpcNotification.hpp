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
#ifndef _PROGRESSUPDATERPCNOTIFICATION_HPP
#define _PROGRESSUPDATERPCNOTIFICATION_HPP

#include <cstddef>
#include <boost/filesystem/path.hpp>

#include <common/BasicTypes.hpp>
#include <common/rpc/AbstractRpcNotification.hpp>
#include <common/stateprov/StateProviderConfig.hpp>

namespace tibee
{

/**
 * Progress update RPC notification.
 *
 * This message is normally sent periodically to progress update
 * listeners to show progress updates to the user during a cache
 * construction.
 *
 * @author Philippe Proulx
 */
class ProgressUpdateRpcNotification :
    public common::AbstractRpcNotification
{
public:
    /**
     * Builds a progress update RPC notification.
     *
     * @param processedEvents Number of processed events.
     */
    ProgressUpdateRpcNotification();

    /**
     * Sets the number of processed events so far.
     *
     * @param processedEvents Number of processed events so far
     */
    void setProcessedEvents(unsigned int processedEvents)
    {
        _processedEvents = processedEvents;
    }

    /**
     * Returns the number of processed events so far.
     *
     * @returns Processed events so far
     */
    std::size_t getProcessedEvents() const
    {
        return _processedEvents;
    }

    /**
     * Sets the trace set begin timestamp.
     *
     * @param beginTs Trace set begin timestamp
     */
    void setBeginTs(common::timestamp_t beginTs)
    {
        _beginTs = beginTs;
    }

    /**
     * Returns the trace set begin timestamp.
     *
     * @returns Trace set begin timestamp
     */
    common::timestamp_t getBeginTs() const
    {
        return _beginTs;
    }

    /**
     * Sets the trace set end timestamp.
     *
     * @param endTs Trace set end timestamp
     */
    void setEndTs(common::timestamp_t endTs)
    {
        _endTs = endTs;
    }

    /**
     * Returns the trace set end timestamp.
     *
     * @returns Trace set end timestamp
     */
    common::timestamp_t getEndTs() const
    {
        return _endTs;
    }

    /**
     * Sets the current timestamp.
     *
     * @param curTs Current timestamp
     */
    void setCurTs(common::timestamp_t curTs)
    {
        _curTs = curTs;
    }

    /**
     * Returns the current timestamp.
     *
     * @returns Trace set current timestamp
     */
    common::timestamp_t getCurTs() const
    {
        return _curTs;
    }

    /**
     * Sets the number of state changes so far.
     *
     * @param stateChanges State changes so far
     */
    void setStateChanges(std::size_t stateChanges)
    {
        _stateChanges = stateChanges;
    }

    /**
     * Returns the number of state changes so far.
     *
     * @returns Number of state changes so far
     */
    unsigned int getStateChanges() const
    {
        return _stateChanges;
    }

    /**
     * Sets the elapsed time for the whole process.
     *
     * @param ms Number of elapsed milliseconds since beginning of process
     */
    void setElapsedTime(std::size_t ms)
    {
        _elapsedTimeMs = ms;
    }

    /**
     * Returns the number of elapsed milliseconds since the beginning of
     * the whole process.
     *
     * @returns Elapsed time since beginning of process in milliseconds
     */
    std::size_t getElapsedTime() const
    {
        return _elapsedTimeMs;
    }

    /**
     * Sets the traces paths used to build the caches.
     *
     * @param tracesPaths Trace paths
     */
    void setTracesPaths(const std::vector<boost::filesystem::path>& tracesPaths)
    {
        _tracesPaths = tracesPaths;
    }

    /**
     * Returns the traces paths used to build the caches.
     *
     * @returns Trace paths
     */
    const std::vector<boost::filesystem::path>& getTracesPaths() const
    {
        return _tracesPaths;
    }

    /**
     * Sets the state providers configurations used to build the caches.
     *
     * @param stateProviders State providers configurations
     */
    void setStateProviders(const std::vector<common::StateProviderConfig>& stateProviders)
    {
        _stateProviders = stateProviders;
    }

    /**
     * Returns the state providers configurations used to build the caches.
     *
     * @returns State providers configurations
     */
    const std::vector<common::StateProviderConfig>& getStateProviders() const
    {
        return _stateProviders;
    }

private:
    unsigned int _processedEvents;
    common::timestamp_t _beginTs;
    common::timestamp_t _endTs;
    common::timestamp_t _curTs;
    std::size_t _elapsedTimeMs;
    std::size_t _stateChanges;
    std::vector<boost::filesystem::path> _tracesPaths;
    std::vector<common::StateProviderConfig> _stateProviders;
};

}

#endif // _PROGRESSUPDATERPCNOTIFICATION_HPP
