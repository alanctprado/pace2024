/******************************************************************************
 * Top contributors (to current version):
 *   Kaio Vieira, Gabriel Ubiratan
 *
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 *
 * Data Structure to store and index information about the instance being solved
 */
#include "oracle.h"
#include "bipartite_graph.h"

#include <stdexcept>

namespace banana {
namespace solver {

Oracle::Oracle(const graph::BipartiteGraph G) : m_graph(G), m_crossing_matrix(G)
{
    m_graph.sortAdjacencyList();
}

std::vector<int> Oracle::neighborhood(int b_vertex) const {
    return m_graph.neighborhood(m_graph.countVerticesA() + b_vertex);
}

std::pair<int, int> Oracle::getInterval(int b_vertex) const {
    const auto &neighbors = neighborhood(b_vertex);
    return {neighbors.front(), neighbors.back()};
}

std::vector<std::pair<int, int>> Oracle::getIntervals(std::vector<int> b_vertices) const {
    std::vector<std::pair<int, int>> intervals;
    for (int b : b_vertices)
        intervals.push_back(getInterval(b));
    return intervals;
}

int Oracle::getCrossings(int i, int j, int w_i, int w_j) const {
    throw std::runtime_error("Not yet implemented.");
}

} // Namespace solver
} // namespace banana
