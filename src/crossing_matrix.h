/******************************************************************************
 * Top contributors (to current version):
 *   Heitor Leite
 *
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 *
 * Crossing matrix.
 */

#ifndef __PACE2024__CROSSING_MATRIX_H
#define __PACE2024__CROSSING_MATRIX_H

#include "bipartite_graph.h"
#include <unordered_map>
#include <map>

namespace banana {
namespace crossing {

/** TODO: Document this class
  */
class CrossingMatrix
{
public:
  CrossingMatrix(graph::BipartiteGraph graph);
  ~CrossingMatrix() = default;

  // NOTE: returns -1 if pair is not orientable
  int operator()(int u, int v) const;
  static std::vector<std::unordered_map<int, int>>
  getIntervals(const graph::BipartiteGraph &graph);
  std::vector<std::pair<int, int>> getOrientablePairs();

protected:
  /* Is this the best way to hash pairs? It works fine assuming size_t is 8
   * bytes and int 4 bytes */
  struct pair_hash
  {
    size_t operator()(const std::pair<int, int> &p) const
    {
      return ((size_t)p.first << 32) | p.second;
    }
  };

  std::unordered_map<std::pair<int, int>, int, pair_hash> m_map;
};

} // namespace crossing
} // namespace banana

#endif // __PACE2024__CROSSING_MATRIX_H
