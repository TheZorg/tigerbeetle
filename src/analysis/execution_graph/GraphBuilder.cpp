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
#include "analysis/execution_graph/GraphBuilder.hpp"

#include <iostream>

#include "base/print.hpp"
#include "base/Constants.hpp"
#include "value/MakeValue.hpp"
#include "value/Utils.hpp"

namespace tibee {
namespace analysis {
namespace execution_graph {

using base::tbendl;
using base::tberror;
using value::MakeValue;

GraphBuilder::GraphBuilder()
    : _ts(0) {
}

GraphBuilder::~GraphBuilder() {
}

void GraphBuilder::SetQuarks(quark::StringQuarkDatabase* quarks)
{
    Q_PARENT_TID = quarks->StrQuark(kParentTid);
    Q_TID = quarks->StrQuark(kTid);
    Q_DURATION = quarks->StrQuark(kDuration);
    Q_NODE_TYPE = quarks->StrQuark(kNodeType);
    Q_START_TIME = quarks->StrQuark(kStartTime);
    Q_STACK_DEPTH = quarks->StrQuark(kStackDepth);
    Q_ARROW_START = quarks->StrQuark(kArrowStart);
}

void GraphBuilder::SetTimestamp(timestamp_t ts)
{
    _ts = ts;
}

bool GraphBuilder::CreateGraph(ThreadId thread, TaskId task, const std::string& description)
{
    // Check whether a graph already exists for this thread.
    if (_threadTasks.find(thread) != _threadTasks.end())
        return false;

    // Create the new graph with a root node.
    auto graph_and_properties = GraphAndProperties::UP {new GraphAndProperties};
    graph_and_properties->description = description;
    auto graph_index = _graphs.size();
    auto& root_node = graph_and_properties->graph.CreateNode();
    _graphs.push_back(std::move(graph_and_properties));
    _taskGraphIndex[task] = graph_index;

    // Create the root task.
    _taskStacks[task].push(&root_node);
    _threadTasks[thread] = task;

    // Set properties for the root task.
    SetProperty(thread, Q_START_TIME, MakeValue(_ts));
    StartTimer(thread, Q_DURATION);
    SetProperty(thread, Q_TID, MakeValue(thread));

    return true;
}

bool GraphBuilder::PushStack(ThreadId thread)
{
    auto task_it = _threadTasks.find(thread);
    if (task_it == _threadTasks.end())
        return false;

    ReadAndResetTimers(thread);

    TaskId task = task_it->second;
    size_t graph_index = _taskGraphIndex[task];

    Node& new_node = _graphs[graph_index]->graph.CreateNode();
    auto& stack = _taskStacks.find(task)->second;
    stack.top()->AddChild(new_node.id());
    stack.push(&new_node);

    SetProperty(thread, Q_START_TIME, MakeValue(_ts));
    SetProperty(thread, Q_STACK_DEPTH, MakeValue(stack.size() - 1));

    return true;
}

bool GraphBuilder::PopStack(ThreadId thread)
{
    auto task_it = _threadTasks.find(thread);
    if (task_it == _threadTasks.end())
        return false;

    ReadAndResetTimers(thread);

    TaskId task = task_it->second;
    auto stack_it = _taskStacks.find(task);
    auto& stack = stack_it->second;
    assert(!stack.empty());
    stack.pop();

    if (stack.empty())
    {
        _taskStacks.erase(stack_it);
        _threadTasks.erase(thread);
        _taskGraphIndex.erase(task);
        _timers.erase(thread);
    }
    else
    {
        SetProperty(thread, Q_START_TIME, MakeValue(_ts));
    }

    return true;
}

bool GraphBuilder::CreateTask(ThreadId parent_thread, TaskId child_task)
{
    auto task_it = _threadTasks.find(parent_thread);
    if (task_it == _threadTasks.end())
        return false;

    ReadAndResetTimers(parent_thread);

    TaskId parent_task = task_it->second;
    size_t graph_index = _taskGraphIndex[parent_task];
    Properties& properties = _graphs[graph_index]->properties;

    // Create the initial node, with info about the arrow.
    Node& new_node = _graphs[graph_index]->graph.CreateNode();

    properties.SetProperty(NodeStepKey(new_node.id(), 0), Q_PARENT_TID, MakeValue(parent_thread));
    properties.SetProperty(NodeStepKey(new_node.id(), 0), Q_ARROW_START, MakeValue(_ts));

    // Add the link from parent task.
    _taskStacks[parent_task].top()->AddChild(new_node.id());
    SetProperty(parent_thread, Q_START_TIME, MakeValue(_ts));

    _taskStacks[child_task].push(&new_node);

    return true;
}

bool GraphBuilder::ScheduleTask(TaskId task, ThreadId thread)
{
    auto taskStackIt = _taskStacks.find(task);
    if (taskStackIt == _taskStacks.end()) {
        tberror() << "Tried to schedule a task " << task << " that doesn't exist." << tbendl();
        return false;
    }
    if (taskStackIt->second.size() != 1) {
        tberror() << "Tried to schedule a task " << task << " with invalid stack size." << tbendl();
        return false;
    }

    _threadTasks[thread] = task;

    SetProperty(thread, Q_START_TIME, MakeValue(_ts));
    StartTimer(thread, Q_DURATION);
    SetProperty(thread, Q_TID, MakeValue(thread));

    return true;
}

bool GraphBuilder::StartTimer(ThreadId thread, quark::Quark timer_name) {
    if (_threadTasks.find(thread) == _threadTasks.end())
        return false;

    _timers[thread].StartTimer(_ts, timer_name);

    return true;
}

bool GraphBuilder::StopTimer(ThreadId thread, quark::Quark timer_name) {
    if (_threadTasks.find(thread) == _threadTasks.end())
        return false;

    uint64_t elapsed_time = 0;
    if (!_timers[thread].ReadAndStopTimer(_ts, timer_name, &elapsed_time))
        return false;

    if (elapsed_time != 0)
        return IncrementProperty(thread, timer_name, elapsed_time);

    return true;
}

uint64_t GraphBuilder::ReadTimer(ThreadId thread, quark::Quark timer_name) {
    if (_threadTasks.find(thread) == _threadTasks.end())
        return false;

    uint64_t elapsed_time = 0;
    if (!_timers[thread].ReadTimer(_ts, timer_name, &elapsed_time))
        return 0;

    return elapsed_time;
}

void GraphBuilder::StopAllTimers()
{
    for (const auto& timer : _timers)
        ReadAndResetTimers(timer.first);
}

bool GraphBuilder::IncrementProperty(ThreadId thread, quark::Quark property, uint64_t increment)
{
    auto task_it = _threadTasks.find(thread);
    if (task_it == _threadTasks.end())
        return false;
    TaskId task = task_it->second;
    auto& stack = _taskStacks.find(task)->second;
    size_t graph_index = _taskGraphIndex[task];

    NodeStepKey key(stack.top()->id(), stack.top()->NumChildren());
    _graphs[graph_index]->properties.IncrementProperty(
        key, property, increment);

    return true;
}

bool GraphBuilder::SetProperty(ThreadId thread, quark::Quark property, value::Value::UP value)
{
    auto task_it = _threadTasks.find(thread);
    if (task_it == _threadTasks.end())
        return false;
    TaskId task = task_it->second;
    auto& stack = _taskStacks.find(task)->second;
    size_t graph_index = _taskGraphIndex[task];

    NodeStepKey key(stack.top()->id(), stack.top()->NumChildren());
    _graphs[graph_index]->properties.SetProperty(
        key, property, std::move(value));

    return true;
}

const value::Value* GraphBuilder::GetProperty(ThreadId thread, quark::Quark property)
{
    auto task_it = _threadTasks.find(thread);
    if (task_it == _threadTasks.end())
        return nullptr;
    TaskId task = task_it->second;
    auto& stack = _taskStacks.find(task)->second;
    size_t graph_index = _taskGraphIndex[task];

    NodeStepKey key(stack.top()->id(), stack.top()->NumChildren());
    return _graphs[graph_index]->properties.GetProperty(key, property);
}

bool GraphBuilder::ReadAndResetTimers(ThreadId thread)
{
    using namespace std::placeholders; 

    auto task_it = _threadTasks.find(thread);
    if (task_it == _threadTasks.end())
        return nullptr;

    TaskId task = task_it->second;
    auto& stack = _taskStacks.find(task)->second;
    size_t graph_index = _taskGraphIndex[task];
    NodeStepKey key(stack.top()->id(), stack.top()->NumChildren());

    auto timer_it = _timers.find(thread);
    timer_it->second.ReadAndResetTimers(
        _ts,
        std::bind(&Properties::IncrementProperty,
                  &_graphs[graph_index]->properties,
                  key,
                  _1,
                  _2));

    return true;
}


}    // namespace execution_graph
}    // namespace analysis
}    // namespace tibee