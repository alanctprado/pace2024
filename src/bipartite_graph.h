/******************************************************************************
 * Top contributors (to current version):
 *   Alan Prado, Heitor Leite
 *
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 *
 * Bipartite graph class.
 */

#ifndef __PACE2024__BIPARTITE_GRAPH_H
#define __PACE2024__BIPARTITE_GRAPH_H

#include "lgraph.h"

namespace banana {
namespace graph {

/**
 * Bipartite graph class.
 *
 * A graph whose vertices can be divided into two disjoint and independent sets
 * A and B, that is, every edge connects a vertex in A to one in B. Suitable
 * for defining methods that are exclusive to or useful on bipartite graphs.
 */
class BipartiteGraph : public LGraph
{
public:
  BipartiteGraph(int n0, int n1);
  BipartiteGraph(const BipartiteGraph &H); // Copy constructor
  ~BipartiteGraph() = default;

  /** Get the number of vertices in part A */
  unsigned countVerticesA() const;
  /** Get the number of vertices in part B */
  unsigned countVerticesB() const;
  /**
   * Builds a crossing matrix (edge crosses between each pair of vertices)
   * indexed by the vertices in part B.
   * */
  std::vector<std::vector<int>> buildCrossingMatrix() const; 

  /* Get vertices in partitions A and B */
  std::vector<int> getA() const;
  std::vector<int> getB() const;

protected:
  std::vector<int> m_partA, m_partB;
};

} // namespace graph
} // namespace banana

#endif  // __PACE2024__BIPARTITE_GRAPH_H
