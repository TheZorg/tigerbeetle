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

#include <common/utils/print.hpp>
#include <tibeecompare/ex/InvalidTrace.hpp>
#include "GraphBuilder.hpp"

#define THIS_MODULE "graphbuilder"

namespace tibee
{

using common::tbmsg;
using common::tbendl;
using timeline_graph::TimelineGraph;

namespace {
const char kTimestampPropertyName[] = "timestamp";
const char kDurationPropertyName[] = "duration";
}

GraphBuilder::GraphBuilder() {
}

GraphBuilder::~GraphBuilder() {
}

std::unique_ptr<TimelineGraph> GraphBuilder::TakeGraph() {
  return std::move(_graph);
}

std::unique_ptr<NodeProperties> GraphBuilder::TakeProperties() {
  return std::move(_properties);
}

bool GraphBuilder::onStartImpl(const common::TraceSet* traceSet) {
   using namespace std::placeholders; 

   _graph = std::unique_ptr<TimelineGraph>(new TimelineGraph());
   _properties = std::unique_ptr<NodeProperties>(new NodeProperties());
   _last_node_for_tid.clear();

  _eventHandlerSelector.setTraceSet(traceSet);
  _eventHandlerSelector.registerEventCallback(
      "lttng-kernel", "sys_execve",
      std::bind(&GraphBuilder::onSysExecve, this, _1));
  _eventHandlerSelector.registerEventCallback(
      "lttng-kernel", "sched_process_fork",
      std::bind(&GraphBuilder::onSchedProcessFork, this, _1));
  _eventHandlerSelector.registerEventCallback(
      "lttng-kernel", "sched_process_exit",
      std::bind(&GraphBuilder::onSchedProcessExit, this, _1));

  // TODO: This is a temporary hack.
  _graph->CreateNode();
  _last_node_for_tid[20550] = 0;
  _last_node_for_tid[20596] = 0;

  return true;
}

void GraphBuilder::onEventImpl(const common::Event& event) {
  _eventHandlerSelector.onEvent(event);
}

bool GraphBuilder::onStopImpl() {
  return true;
}

bool GraphBuilder::onSysExecve(const common::Event& event) {
  std::string filename = event["filename"].asString();

  tbmsg(THIS_MODULE) << "execve " << filename << tbendl();
  return true;
}

bool GraphBuilder::onSchedProcessFork(const common::Event& event) {
  uint64_t parent_tid = event["parent_tid"].asUint();
  uint64_t child_tid = event["child_tid"].asUint();

  tbmsg(THIS_MODULE) << "fork " << parent_tid << " -> " << child_tid
                     << tbendl();

  auto parent_last_node_it = _last_node_for_tid.find(parent_tid);
  if (parent_last_node_it == _last_node_for_tid.end())
    return true;

  // Update duration of the last node.
  uint64_t last_node_ts = _properties->GetProperty(
      parent_last_node_it->second,
      kTimestampPropertyName).asUint64();
  uint64_t last_node_duration = event.getTimestamp() - last_node_ts;
  _properties->SetProperty(
      parent_last_node_it->second,
      kDurationPropertyName,
      last_node_duration);

  // Create the branch node to the parent thread.
  auto& branch_node = _graph->CreateNode();
  _properties->SetProperty(branch_node.id(),
                           kTimestampPropertyName,
                           event.getTimestamp());
  _graph->GetNode(parent_last_node_it->second).set_horizontal_child(
      branch_node.id());

  // Create the first node of the child thread.
  auto& child_node = _graph->CreateNode();
  _properties->SetProperty(child_node.id(),
                           kTimestampPropertyName,
                           event.getTimestamp());
  branch_node.set_vertical_child(child_node.id());

  // Keep track of the last node for each thread.
  _last_node_for_tid[parent_tid] = branch_node.id();
  _last_node_for_tid[child_tid] = child_node.id();

  return true;
}

bool GraphBuilder::onSchedProcessExit(const common::Event& event) {
  uint64_t tid = event["tid"].asUint();

  tbmsg(THIS_MODULE) << "exit " << tid << tbendl();

  auto last_node_it = _last_node_for_tid.find(tid);
  if (last_node_it == _last_node_for_tid.end())
    return true;

  // Update duration of the last node.
  uint64_t last_node_ts = _properties->GetProperty(
      last_node_it->second,
      kTimestampPropertyName).asUint64();
  uint64_t last_node_duration = event.getTimestamp() - last_node_ts;
  _properties->SetProperty(
      last_node_it->second,
      kDurationPropertyName,
      last_node_duration);

  // Create the exit node.
  auto& exit_node = _graph->CreateNode();
  _graph->GetNode(last_node_it->second).set_horizontal_child(exit_node.id());

  return true;
}

}
