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
#include <cassert>

namespace banana {
namespace solver {

Oracle::Oracle() : m_graph(0, 0), m_crossing_matrix(m_graph) {}

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

/** TODO: Verify how to use crossing_matrix */
int Oracle::getCrossings(int i, int j, F w_i, F w_j) const {
    F answer = w_i * w_j * F(m_crossing_matrix(i, j));
    assert(answer.den() == 1);
    return answer.num();
}

} // Namespace solver
} // namespace banana

// Explain what this code does
