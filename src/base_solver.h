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

#ifndef __PACE2024__BASE_SOLVER_HPP
#define __PACE2024__BASE_SOLVER_HPP

#include "bipartite_graph.h"
#include "ip_solver.h"

#include <memory>

/**
 * Implements the main solver logic
 */
class BaseSolver
{
 public:
  BaseSolver(BipartiteGraph graph);
  ~BaseSolver() = default;
  /** Integer programming solver. */
  std::unique_ptr<IntegerProgrammingSolver> m_ipSolver;
  const BipartiteGraph m_graph;

};

#endif  // __PACE2024__BASE_SOLVER_HPP
