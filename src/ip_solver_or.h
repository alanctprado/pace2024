/******************************************************************************
 * Top contributors (to current version):
 *   Luis Higino
 *
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 *
 * OR-Tools extension of the integer programming solver
 */

#ifndef __PACE2024__OR_TOOLS_SOLVER_HPP
#define __PACE2024__OR_TOOLS_SOLVER_HPP

#ifdef USE_OR_TOOLS

#include "bipartite_graph.h"
#include "ip_solver.h"
#include "options.h"
#include "ortools/linear_solver/linear_solver.h"

#include <vector>

namespace banana {
namespace solver {
namespace ip {

using namespace operations_research; // OR-Tools

class OrToolsSolver
    : public IntegerProgrammingSolver<MPSolver, std::vector<MPVariable>>
{
public:
  OrToolsSolver(graph::BipartiteGraph G,
                const options::IPSubSolverMode &subSolver);
  ~OrToolsSolver() = default;

  int simple() override;
  int shorter() override;
  int quadratic() override;
  int vini() override;

  void xPrefix(MPSolver *model, std::vector<MPVariable> &vars) override;
  void yPrefix(MPSolver *model, std::vector<MPVariable> &vars) override;

protected:
  options::IPSubSolverMode sub_solver;
};

} // namespace ip
} // namespace solver
} // namespace banana

#endif  //

#endif  // __PACE2024__OR_TOOLS_SOLVER_HPP
