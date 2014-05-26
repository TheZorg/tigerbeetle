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
#include <iostream>
#include <memory>
#include <boost/filesystem/path.hpp>

#include <common/trace/EventValueType.hpp>
#include <common/trace/AbstractEventValue.hpp>
#include "AbstractCacheBuilder.hpp"
#include "StateHistoryBuilder.hpp"
#include "AbstractStateProvider.hpp"
#include "DynamicLibraryStateProvider.hpp"
#include "PythonStateProvider.hpp"
#include "ex/UnknownStateProviderType.hpp"

namespace bfs = boost::filesystem;

namespace tibee
{

StateHistoryBuilder::StateHistoryBuilder(const bfs::path& dir,
                                         const std::vector<bfs::path>& providersPaths) :
    AbstractCacheBuilder {dir},
    _providersPaths {providersPaths}
{
    std::cout << "state history builder: opening files for writing" << std::endl;

    for (auto& providerPath : providersPaths) {
        // known providers are right here for the moment
        auto extension = providerPath.extension();

        AbstractStateProvider::UP stateProvider;

        if (extension == ".so" || extension == ".dll" || extension == ".dylib") {
            stateProvider = std::move(AbstractStateProvider::UP {
                new DynamicLibraryStateProvider {providerPath}
            });
        } else if (extension == ".py") {
            stateProvider = std::move(AbstractStateProvider::UP {
                new PythonStateProvider {providerPath}
            });
        } else {
            throw ex::UnknownStateProviderType {providerPath};
        }

        _providers.push_back(std::move(stateProvider));
    }
}

StateHistoryBuilder::~StateHistoryBuilder()
{
    std::cout << "state history builder: closing files" << std::endl;
}

bool StateHistoryBuilder::onStart(const std::shared_ptr<const common::TraceSet>& traceSet)
{
    std::cout << "state history builder: starting" << std::endl;

    // also notify each state provider
    for (auto& provider : _providers) {
        provider->onStart(traceSet);
    }

    return true;
}

void StateHistoryBuilder::onEvent(const common::Event& event)
{
    std::cout << "state history builder: processing event: " << event.getName() << std::endl;

    // also notify each state provider
    for (auto& provider : _providers) {
        provider->onEvent(event);
    }
}

bool StateHistoryBuilder::onStop()
{
    std::cout << "state history builder: stopping" << std::endl;

    // also notify each state provider
    for (auto& provider : _providers) {
        provider->onStop();
    }

    return true;
}

}