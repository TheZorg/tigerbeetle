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
#ifndef _TIBEE_CRITICAL_CRITICALGRAPH_HPP
#define _TIBEE_CRITICAL_CRITICALGRAPH_HPP

#include <boost/noncopyable.hpp>
#include <unordered_map>
#include <vector>

#include "critical/CriticalEdge.hpp"
#include "critical/CriticalNode.hpp"
#include "critical/CriticalPath.hpp"

namespace tibee
{
namespace critical
{

/**
 * Critical node.
 *
 * @author Francois Doray
 */
class CriticalGraph :
    boost::noncopyable
{
public:
    typedef std::vector<CriticalNode*> OrderedNodes;
    typedef std::unordered_map<uint32_t, std::unique_ptr<OrderedNodes>> TidToNodesMap;

    CriticalGraph();
    ~CriticalGraph();
    
    CriticalNode* CreateNode(timestamp_t ts, uint32_t tid);

    const CriticalNode* GetNodeIntersecting(timestamp_t ts, uint32_t tid) const;
    CriticalNode* GetLastNodeForThread(uint32_t tid);

    CriticalEdgeId CreateHorizontalEdge(
        CriticalEdgeType type,
        CriticalNode* from,
        CriticalNode* to);

    CriticalEdgeId CreateVerticalEdge(
        CriticalNode* from,
        CriticalNode* to);

    const CriticalEdge& GetEdge(CriticalEdgeId id) const {
        return _edges[id];
    }

    bool ComputeCriticalPath(
        const CriticalNode* from,
        const CriticalNode* to,
        const std::unordered_set<uint32_t>& tids,
        CriticalPath* path) const;

    size_t NumThreads() const {
        return _tid_to_nodes.size();
    }

    TidToNodesMap::const_iterator threads_begin() const {
        return _tid_to_nodes.begin();
    }
    TidToNodesMap::const_iterator threads_end() const {
        return _tid_to_nodes.end();
    }

private:
    bool TopologicalSort(
        const CriticalNode* from,
        const CriticalNode* to,
        std::vector<const CriticalNode*>* out) const;

    void GetInitialNodeForEachThread(
        timestamp_t ts,
        std::vector<const CriticalNode*>* nodes) const;

    // Timestamp of the last node created.
    timestamp_t _last_ts;

    // Nodes.
    std::vector<CriticalNode::UP> _nodes;

    // Nodes, organized by tid and timestamp.
    TidToNodesMap _tid_to_nodes;

    // Edges.
    std::vector<CriticalEdge> _edges;
};

}
}

#endif // _TIBEE_CRITICAL_CRITICALGRAPH_HPP
