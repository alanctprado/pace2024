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
#include "../lp_solve_5.5/lp_lib.h"

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
  /** Simple formulation */
  int simpleLPSolve();

  /** Shorter simple formulation */
  std::pair<int, bool> triangularIndex(int i, int j);
  int shorterSimpleLPSolve();

  /** Quadratic formulation */
  int yIndex(int i, int j, int n);
  int quadraticLPSolve();
  int viniLPSolve();

  /** Prefix constraints */
  void computeDeltas();
  void xPrefixLPSolve(lprec* lp, std::vector<double>& c);
  void yPrefixLPSolve(lprec* lp, std::vector<double>& c);
};

} // namespace ip
} // namespace solver
} // namespace banana

#endif // __PACE2024__IP_SOLVER_HPP
