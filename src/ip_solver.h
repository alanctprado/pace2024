/******************************************************************************
 * Top contributors (to current version):
 *   Alan Prado
 *
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 *
 * Integer programming solver.
 */

#ifndef __PACE2024__IP_SOLVER_HPP
#define __PACE2024__IP_SOLVER_HPP

#include "bipartite_graph.h"
#include "meta_solver.h"

#include <string>

namespace banana {
namespace solver {
namespace ip {

/**
 * This class defines an integer programming solver for the OSCM problem. The
 * objective function and constraints are specified in the implementation.
 *
 * Currently, the integer programming solvers available are:
 *   - LP Solve
 */
class IntegerProgrammingSolver : public MetaSolver<int>
{
 public:
  IntegerProgrammingSolver(graph::BipartiteGraph G);
  ~IntegerProgrammingSolver() = default;
  int solve() override;

 protected:
  /** Formulation 1 */
  int solveWithLPSolve1();
  /** Formulation 2 */
  std::pair<int, bool> index2(int i, int j);
  int solveWithLPSolve2();
  std::string m_solver;
};

} // namespace ip
} // namespace solver
} // namespace banana

#endif  // __PACE2024__IP_SOLVER_HPP
