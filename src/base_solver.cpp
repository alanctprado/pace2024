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
#include "ip_solver_or.h"
#include "ip_solver_gurobi.h"
#include "ip_solver_lpsolve.h"
#include "options.h"
#include "utils.h"

#include <memory>

namespace banana {
namespace solver {

BaseSolver::BaseSolver(graph::BipartiteGraph graph) : m_graph(graph)
{
  const auto &ip_solver = Environment::options().ip.solverMode;
  const auto &ip_sub_solver = Environment::options().ip.subSolverMode;
  switch (ip_solver)
  {
  case options::IPSolverMode::LPSOLVE:
    m_ipSolver = std::make_unique<ip::LPSolveSolver>(graph);
    break;
  #ifdef USE_GUROBI
  case options::IPSolverMode::GUROBI:
    m_ipSolver = std::make_unique<ip::GurobiSolver>(graph);
    break;
  #endif
  #ifdef USE_OR_TOOLS
  case options::IPSolverMode::OR_TOOLS:
    m_ipSolver = std::make_unique<ip::OrToolsSolver>(graph, ip_sub_solver);
    break;
  #endif
  default:
    throw std::invalid_argument("Invalid IP Solver!");

  }
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
  for (int vertex : order)
  {
    std::cerr << vertex + 1 << "\n";
  }
  assert(m_ipSolver->verify(order, crossings));
  if (Environment::options().verify.verifyMode ==
      banana::options::VerifyMode::COMPLETE)
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
