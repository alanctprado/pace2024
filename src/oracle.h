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
#ifndef __PACE_2024_ORACLE_HPP
#define __PACE_2024_ORACLE_HPP

#include "bipartite_graph.h"
#include "crossing_matrix.h"

namespace banana {
namespace solver {

/**
 * Oracle Data Structure.
 *
 * Stores the crossing matrix and the adjacency list of the instance,
 * to be consulted by 'subgraphs' created during the solving proccess.
 */
class Oracle
{
public:
    Oracle(const graph::BipartiteGraph);

    /**
     * Returns the real label name of the i-th vertex of the partition B.
     *
     * The Oracle assumes the indexing of the vertex of b as 0-based
     * to be more convenient, as we rarely index vertices of the partition A.
     */
    int getLabel(int b_vertex) const;

    /** Gets all neighbors of b */
    std::vector<int> neighborhood(int b_vertex) const;
    /** Gets [min, max] neighbors of b */
    std::pair<int, int> getInterval(int b_vertex) const;
    /** Gets [min, max] neighbors of the given vertices */
    std::vector<std::pair<int, int>> getIntervals(std::vector<int> b_vertices) const;

    /**
     * Number of crossings between i and j.
     *
     * TODO: change w_i and w_j to a fraction type.
     * TODO: assert that this number is an integer.
     **/
    int getCrossings(int i, int j, int w_i = 1, int w_j = 1) const;

protected:
    graph::BipartiteGraph m_graph;
    crossing::CrossingMatrix m_crossing_matrix;
};

} // namespace solver
} // namespace banana

#endif // __PACE_2024_ORACLE_HPP
