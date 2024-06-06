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
 * Meta solver for the one-sided crossing minimization problem. An abstract
 * class to be extended for the various solvers.
 */

#ifndef __PACE2024__META_SOLVER_HPP
#define __PACE2024__META_SOLVER_HPP

#include "bipartite_graph.h"
#include "fenwick_tree.h"
#include "oracle.h"

#include <algorithm>
#include <iostream>
#include <cassert>
#include <vector>

namespace banana {
namespace solver {

/**
 * Abstract solver class
 */
class MetaSolver
{
public:
  MetaSolver(Oracle::SubProblem G) : m_instance(G) {}
  virtual ~MetaSolver() {}

  virtual int solve() = 0;
  void explain(std::vector<Oracle::Vertex> &order);

protected:
  Oracle::SubProblem const m_instance;
  Oracle::SubProblem m_order;
};

} // namespace solver
} // namespace banana

#endif // __PACE2024__META_SOLVER_HPP
