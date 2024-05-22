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

#include "bipartite_graph.h"
#include "ip_solver.h"
#include "../or-tools/ortools/linear_solver/linear_solver.h"

#include <vector>

namespace banana {
namespace solver {
namespace ip {

using namespace operations_research; // OR-Tools

enum class OrSolver
{
  SCIP,
  GLOP,
  CBC,
  HIGHS,
};

class OrToolsSolver : public IntegerProgrammingSolver<MPSolver, std::vector<MPVariable>>
{
public:
  OrToolsSolver(graph::BipartiteGraph G);
  ~OrToolsSolver() = default;

  int simple() override;
  int shorter() override;
  int quadratic() override;
  int vini() override;

  void xPrefix(MPSolver* model, std::vector<MPVariable>& vars) override;
  void yPrefix(MPSolver* model, std::vector<MPVariable>& vars) override;
};

} // namespace ip
} // namespace solver
} // namespace banana

#endif // __PACE2024__OR_TOOLS_SOLVER_HPP
