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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.    See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tigerbeetle.    If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _TIBEE_EXECUTION_NODEPAIR_HPP
#define _TIBEE_EXECUTION_NODEPAIR_HPP

#include <boost/functional/hash/hash.hpp>
#include <utility>

#include "execution/Node.hpp"

namespace tibee {
namespace execution {

class NodePair {
 public:
    friend struct std::hash<NodePair>;

    NodePair()
        : pair_(kInvalidNodeId, kInvalidNodeId) {
    }

    NodePair(NodeId node_id_a, NodeId node_id_b)
        : pair_(node_id_a, node_id_b) {
    }

    NodeId node_id_a() const {
        return pair_.first; 
    }
    
    NodeId node_id_b() const {
        return pair_.second;
    }

    bool operator==(const NodePair& other) const {
        return pair_ == other.pair_;
    }

    bool operator!=(const NodePair& other) const {
        return pair_ != other.pair_;
    }

    bool operator<(const NodePair& other) const {
        if (node_id_a() < other.node_id_a())
            return true;
        if (node_id_a() > other.node_id_a())
            return false;
        return node_id_b() < other.node_id_b();
    }

 private:
    std::pair<NodeId, NodeId> pair_;
};

}    // namespace execution
}    // namespace tibee

namespace std {

template <>
struct hash<tibee::execution::NodePair> {
    size_t operator()(const tibee::execution::NodePair& node_pair) const {
        return boost::hash_value(node_pair.pair_);
    }
};

}    // namespace std

#endif    // _TIBEE_EXECUTION_NODEPAIR_HPP