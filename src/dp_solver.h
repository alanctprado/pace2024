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
 * Dynamic programming solver.
 */

#ifndef __PACE2024__DP_SOLVER_HPP
#define __PACE2024__DP_SOLVER_HPP

#include "bipartite_graph.h"
#include "meta_solver.h"
#include "oracle.h"

namespace banana {
namespace solver {
namespace dp {

class DPSolver : public MetaSolver
{
public:
  DPSolver(Oracle::SubProblem G);
  ~DPSolver() = default;
  int solve() override;
  void explain(std::vector<Oracle::Vertex> &order);
  double estimateTime();
  double estimateMemory();

protected:
  enum Event { INSERT, FORGET };
  std::vector<std::pair<Event, int>> m_decomposition;
  std::vector<int> m_bag_sizes;
  void decompose();
};

} // namespace dp
} // namespace solver
} // namespace banana

#endif // __PACE2024__DP_SOLVER_HPP
