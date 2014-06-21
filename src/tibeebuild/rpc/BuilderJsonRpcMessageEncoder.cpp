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
#include "BuilderJsonRpcMessageEncoder.hpp"

namespace tibee
{

BuilderJsonRpcMessageEncoder::BuilderJsonRpcMessageEncoder()
{
}

std::unique_ptr<std::string>
BuilderJsonRpcMessageEncoder::encodeProgressUpdateRpcNotification(const ProgressUpdateRpcNotification& object)
{
    return this->encodeNotification(object,
                                    BuilderJsonRpcMessageEncoder::encodeProgressUpdateRpcNotificationParams);
}

bool BuilderJsonRpcMessageEncoder::encodeProgressUpdateRpcNotificationParams(const common::AbstractRpcMessage& msg,
                                                                             ::yajl_gen yajlGen)
{
    auto pu = static_cast<const ProgressUpdateRpcNotification&>(msg);

    // keys
    TIBEE_DEF_YAJL_STR(PROCESSED_EVENTS, "processed-events");
    TIBEE_DEF_YAJL_STR(TRACES_BEGIN_TS, "traces-begin-ts");
    TIBEE_DEF_YAJL_STR(TRACES_END_TS, "traces-end-ts");
    TIBEE_DEF_YAJL_STR(TRACES_CUR_TS, "traces-cur-ts");
    TIBEE_DEF_YAJL_STR(STATE_CHANGES, "state-changes");
    TIBEE_DEF_YAJL_STR(TRACES_PATHS, "traces-paths");
    TIBEE_DEF_YAJL_STR(STATE_PROVIDERS, "state-providers");
    TIBEE_DEF_YAJL_STR(ELAPSED_TIME, "elapsed-time");

    // open object
    ::yajl_gen_map_open(yajlGen);

    // processed events
    ::yajl_gen_string(yajlGen, PROCESSED_EVENTS, PROCESSED_EVENTS_LEN);
    ::yajl_gen_integer(yajlGen, pu.getProcessedEvents());

    // trace set begin timestamp
    ::yajl_gen_string(yajlGen, TRACES_BEGIN_TS, TRACES_BEGIN_TS_LEN);
    ::yajl_gen_integer(yajlGen, static_cast<long long int>(pu.getBeginTs()));

    // trace set end timestamp
    ::yajl_gen_string(yajlGen, TRACES_END_TS, TRACES_END_TS_LEN);
    ::yajl_gen_integer(yajlGen, static_cast<long long int>(pu.getEndTs()));

    // trace set current timestamp
    ::yajl_gen_string(yajlGen, TRACES_CUR_TS, TRACES_CUR_TS_LEN);
    ::yajl_gen_integer(yajlGen, static_cast<long long int>(pu.getCurTs()));

    // elapsed time (ms)
    ::yajl_gen_string(yajlGen, ELAPSED_TIME, ELAPSED_TIME_LEN);
    ::yajl_gen_integer(yajlGen, static_cast<long long int>(pu.getElapsedTime()));

    // state changes
    ::yajl_gen_string(yajlGen, STATE_CHANGES, STATE_CHANGES_LEN);
    ::yajl_gen_integer(yajlGen, pu.getStateChanges());

    // traces paths
    ::yajl_gen_string(yajlGen, TRACES_PATHS, TRACES_PATHS_LEN);
    ::yajl_gen_array_open(yajlGen);

    const auto& tracesPaths = pu.getTracesPaths();

    for (const auto& path : tracesPaths) {
        const auto& pathStr = path.string();

        ::yajl_gen_string(yajlGen,
                          reinterpret_cast<const unsigned char*>(pathStr.c_str()),
                          pathStr.size());
    }

    ::yajl_gen_array_close(yajlGen);

    // state providers
    const auto& stateProvidersConfigs = pu.getStateProviders();
    ::yajl_gen_string(yajlGen, STATE_PROVIDERS, STATE_PROVIDERS_LEN);
    ::yajl_gen_array_open(yajlGen);

    for (const auto& stateProviderConfig : stateProvidersConfigs) {
        const auto& name = stateProviderConfig.getName();

        ::yajl_gen_string(yajlGen,
                          reinterpret_cast<const unsigned char*>(name.c_str()),
                          name.size());
    }

    ::yajl_gen_array_close(yajlGen);

    // close object
    ::yajl_gen_map_close(yajlGen);

    return true;
}

}
