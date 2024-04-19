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
 * Base solver for the one-sided crossing minimization problem
 */

#include "base_solver.h"

#include <iostream>

namespace banana {
namespace solver {

BaseSolver::BaseSolver(graph::BipartiteGraph graph)
    : m_ipSolver(new ip::IntegerProgrammingSolver(graph)),
      m_graph(graph)
{
}

void BaseSolver::runBanana()
{
  int crossings = m_ipSolver->solve();
  std::vector<int> order;
  m_ipSolver->explain(order);
  for (int vertex : order)
  {
    std::cout << vertex << "\n";
  }
}

} // namespace solver
} // namespace banana
