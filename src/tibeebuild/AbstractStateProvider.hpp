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
#ifndef _ABSTRACTSTATEPROVIDER_HPP
#define _ABSTRACTSTATEPROVIDER_HPP

#include <boost/filesystem.hpp>

#include <common/state/CurrentState.hpp>
#include "ITracePlaybackListener.hpp"

namespace tibee
{

/**
 * An abstract state provider. Any state provider must inherit
 * this class.
 *
 * @author Philippe Proulx
 */
class AbstractStateProvider
{
public:
    /// Unique pointer to abstract state provider
    typedef std::unique_ptr<AbstractStateProvider> UP;

public:
    /**
     * Builds a state provider.
     *
     * @param path State provider path
     */
    AbstractStateProvider(const boost::filesystem::path& path);

    virtual ~AbstractStateProvider();

    /**
     * Called before processing any event.
     *
     * @param state Current state (empty when getting it)
     */
    virtual void onInit(common::CurrentState& state) = 0;

    /**
     * New event notification.
     *
     * @param state Current state
     * @param event New event
     */
    virtual void onEvent(common::CurrentState& state, const common::Event& event) = 0;

    /**
     * Called after having processed all events.
     *
     * @param state Current state
     */
    virtual void onFini(common::CurrentState& state) = 0;

    /**
     * Returns this provider path.
     *
     * @returns Provider path
     */
    const boost::filesystem::path& getPath() const
    {
        return _path;
    }

private:
    boost::filesystem::path _path;
};

}

#endif // _ABSTRACTSTATEPROVIDER_HPP
