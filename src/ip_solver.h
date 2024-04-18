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
 * Integer programming solver. TO-DO.
 */

#ifndef __PACE2024__IP_SOLVER_HPP
#define __PACE2024__IP_SOLVER_HPP

#include "bipartite_graph.h"
#include "meta_solver.h"

/**
 * TO-DO
 */
class IntegerProgrammingSolver : MetaSolver<int>
{
 public:
  IntegerProgrammingSolver(BipartiteGraph G);
  ~IntegerProgrammingSolver() = default;

  int solve() override;
};

#endif  // __PACE2024__IP_SOLVER_HPP
