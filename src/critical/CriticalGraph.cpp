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
#include "critical/CriticalGraph.hpp"

#include <algorithm>
#include <assert.h>
#include <queue>
#include <unordered_set>

#include "base/print.hpp"

namespace tibee
{
namespace critical
{

namespace
{

using base::tbendl;
using base::tberror;

struct NodeTsComparator
{
    bool operator() (const CriticalNode* node, timestamp_t ts) const
    {
        return node->ts() < ts;
    }
};

struct NodeDistance
{
    NodeDistance() : distance(0), edge(kInvalidCriticalEdgeId) {}
    NodeDistance(size_t distance, CriticalEdgeId edge) : distance(distance), edge(edge) {}
    size_t distance;
    CriticalEdgeId edge;

};

const size_t kHugeDistance = -1;

}  // namespace

CriticalGraph::CriticalGraph()
    : _last_ts(0)
{
}

CriticalGraph::~CriticalGraph()
{
}

CriticalNode* CriticalGraph::CreateNode(timestamp_t ts, uint32_t tid)
{
    assert(ts > _last_ts);
    _last_ts = ts;

    // Create node.
    CriticalNode::UP node(new CriticalNode(ts, tid));
    auto node_ptr = node.get();
    _nodes.push_back(std::move(node));

    // Keep track of nodes per thread.
    auto look = _tid_to_nodes.find(tid);
    if (look == _tid_to_nodes.end())
    {
        auto res = _tid_to_nodes.insert(
            TidToNodesMap::value_type {
                tid, std::unique_ptr<OrderedNodes>(new OrderedNodes)});
        assert(res.second == true);
        look = res.first;
    }
    look->second->push_back(node_ptr);

    return node_ptr;
}

CriticalNode* CriticalGraph::GetNodeAfterOrEqual(timestamp_t ts, uint32_t tid)
{
    auto thread_nodes_it = _tid_to_nodes.find(tid);
    if (thread_nodes_it == _tid_to_nodes.end())
        return nullptr;
    const auto& thread_nodes = thread_nodes_it->second;
    NodeTsComparator comparator;
    auto node_it = std::lower_bound(thread_nodes->begin(), thread_nodes->end(), ts, comparator);
    if (node_it == thread_nodes->end())
        return nullptr;
    return *node_it;
}

const CriticalNode* CriticalGraph::GetNodeAfterOrEqual(timestamp_t ts, uint32_t tid) const
{
    auto thread_nodes_it = _tid_to_nodes.find(tid);
    if (thread_nodes_it == _tid_to_nodes.end())
        return nullptr;
    const auto& thread_nodes = thread_nodes_it->second;
    NodeTsComparator comparator;
    auto node_it = std::lower_bound(thread_nodes->begin(), thread_nodes->end(), ts, comparator);
    if (node_it == thread_nodes->end())
        return nullptr;
    return *node_it;
}

CriticalEdgeId CriticalGraph::CreateHorizontalEdge(
    CriticalEdgeType type,
    CriticalNode* from,
    CriticalNode* to)
{
    CriticalEdgeId id = _edges.size();
    _edges.push_back(CriticalEdge(type, from, to));
    from->set_edge(kCriticalEdgeOutHorizontal, id);
    to->set_edge(kCriticalEdgeInHorizontal, id);
    return id;
}

CriticalEdgeId CriticalGraph::CreateVerticalEdge(
    CriticalNode* from,
    CriticalNode* to)
{
    CriticalEdgeId id = _edges.size();
    _edges.push_back(CriticalEdge(CriticalEdgeType::kVertical, from, to));
    from->set_edge(kCriticalEdgeOutVertical, id);
    to->set_edge(kCriticalEdgeInVertical, id);
    return id;
}

bool CriticalGraph::CriticalPath(
        const CriticalNode* from,
        const CriticalNode* to,
        std::vector<CriticalEdgeId>* path) const
{
    assert(from != nullptr);
    assert(to != nullptr);
    assert(path != nullptr);
    assert(path->empty());

    // Topological sort.
    std::vector<const CriticalNode*> topological;
    if (!TopologicalSort(from, to, &topological)) {
        tberror() << "Destination node wasn't found in topological sort." << tbendl();
        return false;
    }

    // Compute maximum distance from destination for each node.
    std::unordered_map<const CriticalNode*, NodeDistance> distances;

    for (auto it = topological.rbegin(); it != topological.rend(); ++it)
    {
        // If the node is the destination, the distance is zero.
        if (*it == to)
        {
            distances[*it] = NodeDistance(0, kInvalidCriticalEdgeId);
            continue;
        }

        // Compute the 2 possible distances.
        size_t horizontal_distance = kHugeDistance;
        CriticalEdgeId horizontal_edge_id = (*it)->edge(kCriticalEdgeOutHorizontal);

        if (horizontal_edge_id != kInvalidCriticalEdgeId)
        {
            const auto& edge = GetEdge(horizontal_edge_id);
            auto distance_look = distances.find(edge.to());
            if (distance_look != distances.end()) {
                horizontal_distance = distance_look->second.distance;
                if (horizontal_distance != kHugeDistance)
                    horizontal_distance += edge.Cost();
            }
        }

        size_t vertical_distance = kHugeDistance;
        CriticalEdgeId vertical_edge_id = (*it)->edge(kCriticalEdgeOutVertical);

        if (vertical_edge_id != kInvalidCriticalEdgeId)
        {
            const auto& edge = GetEdge(vertical_edge_id);
            auto distance_look = distances.find(edge.to());
            if (distance_look != distances.end()) {
                vertical_distance = distance_look->second.distance;
                if (vertical_distance != kHugeDistance)
                    vertical_distance += edge.Cost();
            }
        }

        // Keep the maximum distance which is not invalid.
        if (horizontal_distance != kHugeDistance &&
            (vertical_distance == kHugeDistance || horizontal_distance >= vertical_distance))
        {
            distances[*it] = NodeDistance(horizontal_distance, horizontal_edge_id);
        }
        else if (vertical_distance != kHugeDistance &&
                 (horizontal_distance == kHugeDistance || vertical_distance >= horizontal_distance))
        {
            distances[*it] = NodeDistance(vertical_distance, vertical_edge_id);
        }
    }

    // Retrieve the critical path.
    const CriticalNode* cur = from;
    while (cur != to)
    {
        const NodeDistance& distance = distances[cur];
        path->push_back(distance.edge);
        cur = GetEdge(distance.edge).to();

    }

    return true;
}

bool CriticalGraph::TopologicalSort(
    const CriticalNode* from,
    const CriticalNode* to,
    std::vector<const CriticalNode*>* out) const
{
    assert(out != nullptr);
    assert(out->empty());

    std::unordered_set<CriticalEdgeId> deleted_edges;
    deleted_edges.insert(kInvalidCriticalEdgeId);

    std::unordered_set<const CriticalNode*> inserted_nodes;

    std::queue<const CriticalNode*> queue;

    // Add initial nodes.
    std::vector<const CriticalNode*> initial_nodes;
    GetFirstNodeForEachThreadAfterOrEqual(from->ts(), &initial_nodes);
    for (auto node : initial_nodes) {
        if (node->ts() <= to->ts())
        {
            // Check if the node has prerequisites that are after from->ts().
            auto horizontal_in_id = node->edge(kCriticalEdgeInHorizontal);
            auto vertical_in_id = node->edge(kCriticalEdgeInVertical);
            
            if (horizontal_in_id != kInvalidCriticalEdgeId &&
                GetEdge(horizontal_in_id).from()->ts() >= from->ts())
            {
                continue;
            }

            if (vertical_in_id != kInvalidCriticalEdgeId &&
                GetEdge(vertical_in_id).from()->ts() >= from->ts())
            {
                continue;
            }

            queue.push(node);
        }
    }

    // Topological sort.
    bool found_to = false;

    while (!queue.empty())
    {
        const CriticalNode* cur = queue.front();
        queue.pop();

        if (inserted_nodes.find(cur) != inserted_nodes.end())
            continue;

        // Add node to output vector.
        out->push_back(cur);
        inserted_nodes.insert(cur);
        if (cur == to)
            found_to = true;

        // Remove all outgoing edges of the current nodes.
        auto horizontal_edge_id = cur->edge(kCriticalEdgeOutHorizontal);
        deleted_edges.insert(horizontal_edge_id);
        auto vertical_edge_id = cur->edge(kCriticalEdgeOutVertical);
        deleted_edges.insert(vertical_edge_id);

        // Add to queue adjacent nodes that have no more incoming edges.
        if (horizontal_edge_id != kInvalidCriticalEdgeId)
        {
            const auto& edge = GetEdge(horizontal_edge_id);
            const CriticalNode* node = edge.to();

            if (deleted_edges.find(node->edge(kCriticalEdgeInVertical)) != deleted_edges.end() &&
                node->ts() <= to->ts())
            {
                queue.push(node);
            }
        }
        if (vertical_edge_id != kInvalidCriticalEdgeId)
        {
            const auto& edge = GetEdge(vertical_edge_id);
            const CriticalNode* node = edge.to();

            if (deleted_edges.find(node->edge(kCriticalEdgeInHorizontal)) != deleted_edges.end() &&
                node->ts() <= to->ts())
            {
                queue.push(node);
            }
        }
    }

    return found_to;
}

void CriticalGraph::GetFirstNodeForEachThreadAfterOrEqual(
        timestamp_t ts,
        std::vector<const CriticalNode*>* nodes) const
{
    for (const auto& thread : _tid_to_nodes)
    {
        uint32_t tid = thread.first;
        const CriticalNode* first_node = GetNodeAfterOrEqual(ts, tid);
        if (first_node != nullptr)
            nodes->push_back(first_node);
    }
}

}  // namespace critical
}  // namespace tibee
