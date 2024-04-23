/******************************************************************************
 * Top contributors (to current version):
 *   Alan Prado, Luis Higino
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
#include "environment.h"
#include "options.h"
#include "utils.h"

#include <iostream>
#include <cassert>
#include <string>

namespace banana {
namespace solver {

BaseSolver::BaseSolver(graph::BipartiteGraph graph)
    : m_ipSolver(new ip::IntegerProgrammingSolver(graph)),
      m_graph(graph)
{
}

void BaseSolver::verifySolution(int expected_crossings)
{
  std::string path = Environment::options().verify.verifyPath;
  std::vector<int> order = utils::readSolution<int>(path);
  assert(m_ipSolver->verify(order, expected_crossings));
}

void BaseSolver::runBanana()
{
  int crossings = m_ipSolver->solve();
  std::vector<int> order;
  m_ipSolver->explain(order);
  assert(m_ipSolver->verify(order, crossings));
  if (Environment::options().verify.verifyMode ==
      banana::options::VerifyMode::FULL)
  {
    verifySolution(crossings);
  }
  for (int vertex : order)
  {
    std::cout << vertex + 1 << "\n";
  }
}

} // namespace solver
} // namespace banana
