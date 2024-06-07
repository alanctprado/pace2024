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
#include "fraction_type.h"
#include <utility>
#include <iostream>

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
    Oracle();
    Oracle(const graph::BipartiteGraph);

    typedef library::Fraction<long long> F;
    typedef std::pair<int, F> Vertex;
    typedef std::vector<Oracle::Vertex> SubProblem;

    /**
     * Returns the real label name of the i-th vertex of the partition B.
     *
     * The Oracle assumes the indexing of the vertex of b as 0-based
     * to be more convenient, as we rarely index vertices of the partition A.
     */
    int getLabel(int b_vertex) const;

    unsigned countVerticesA() const;
    /** Returns all neighbors of b */
    std::vector<int> neighborhood(int b_vertex) const;
    /** Returns [min, max] neighbors of b */
    std::pair<int, int> getInterval(int b_vertex) const;
    /** Returns [min, max] neighbors of the given vertices */
    std::vector<std::pair<int, int>>
    getIntervals(std::vector<int> b_vertices) const;

    int degree(int b_vertex) const;

    /** Returns [min, max] neighbors as 2 unordered_maps */
    std::array<std::unordered_map<int, int>, 2>
    getIntervalMaps(const SubProblem &instance) const;

    std::vector<std::pair<int, int>> getIntervals(SubProblem b_vertices) const;

    /** Returns the number of crossings of a permutation of vertices from the
     * partition B */
    int numberOfCrossings(const std::vector<Vertex> &order) const;

    /** Checks if the order has the expected number of crossings */
    bool verify(const std::vector<Vertex> &order, int expected_crossings) const;

    /** Number of crossings between i and j */
    int getCrossings(Vertex v_i, Vertex v_j) const;

    std::vector<std::pair<int, int>>
    getCompressedIntervals(const SubProblem &instance) const;

    /** get all orientable pairs of some subinstance P */
    std::vector<std::pair<int, int>>
    getOrientablePairs(const std::vector<int> &p) const;

    /** get all orientable pairs of some subinstance P */
    std::vector<std::pair<int, int>>
    getOrientablePairs(const SubProblem &instance) const;

    std::vector<std::pair<int, int>> getOrientablePairsIdx(const SubProblem &p) const;

  protected:
    graph::BipartiteGraph m_graph;
    crossing::CrossingMatrix m_crossing_matrix;
};

} // namespace solver
} // namespace banana

#endif // __PACE_2024_ORACLE_HPP
