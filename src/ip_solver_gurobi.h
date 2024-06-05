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
 * Gurobi extension of the integer programming solver
 */

#ifndef __PACE2024__GUROBI_SOLVER_HPP
#define __PACE2024__GUROBI_SOLVER_HPP

#ifdef USE_GUROBI

#include "bipartite_graph.h"
#include "ip_solver.h"
#include "../gurobi/include/gurobi_c++.h"

#include <vector>

namespace banana {
namespace solver {
namespace ip {

/** TODO: document */
class GurobiSolver : public IntegerProgrammingSolver<GRBModel, GRBVar>
{
public:
  GurobiSolver(graph::BipartiteGraph G);
  ~GurobiSolver() = default;

  int simple() override;
  int shorter() override;
  int quadratic() override;
  int vini() override;

  void xPrefix(GRBModel *model, GRBVar &vars) override;
  void yPrefix(GRBModel *model, GRBVar &vars) override;
};

} // namespace ip
} // namespace solver
} // namespace banana

#endif  // USE_GUROBI

#endif //  __PACE2024__GUROBI_SOLVER_HPP
