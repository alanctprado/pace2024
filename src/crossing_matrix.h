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

namespace banana {
namespace crossing {

/** TODO: Document this class
  */
class CrossingMatrix
{
public:
  CrossingMatrix(graph::BipartiteGraph graph);
  ~CrossingMatrix() = default;

    /** TODO: explain */
  int operator()(int u, int v) const;

protected:
  std::unordered_map<int, std::unordered_map<int, int>> m_map;
};

} // namespace crossing
} // namespace banana

#endif  // __PACE2024__CROSSING_MATRIX_H
