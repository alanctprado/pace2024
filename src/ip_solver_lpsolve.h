/******************************************************************************
 * Top contributors (to current version):
 *   Alan Prado, Kaio Vieira
 *
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 *
 * LPSolve extension of the integer programming solver
 */

#ifndef __PACE2024__LPSOLVE_SOLVER_HPP
#define __PACE2024__LPSOLVE_SOLVER_HPP

#include "bipartite_graph.h"
#include "ip_solver.h"
#include "../lp_solve_5.5/lp_lib.h"

#include <vector>

namespace banana {
namespace solver {
namespace ip {

/** TODO: document */
class LPSolveSolver
    : public IntegerProgrammingSolver<lprec, std::vector<double>>
{
public:
  LPSolveSolver(Oracle::SubProblem instance);
  ~LPSolveSolver() = default;

  int simple() override;
  int shorter() override;
  int quadratic() override;
  int vini() override;

  void xPrefix(lprec *program, std::vector<double> &vars) override;
  void yPrefix(lprec *program, std::vector<double> &vars) override;
};

} // namespace ip
} // namespace solver
} // namespace banana

#endif // __PACE2024__LPSOLVE_SOLVER_HPP
