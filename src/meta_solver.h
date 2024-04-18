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
 * Meta solver for the one-sided crossing minimization problem. An abstract
 * class to be extended for the various solvers.
 */

#ifndef __PACE2024__META_SOLVER_HPP
#define __PACE2024__META_SOLVER_HPP

#include "bipartite_graph.h"

#include <cassert>
#include <vector>

/**
 * Abstract solver class
 */
template<class T>
class MetaSolver
{
 public:
  MetaSolver(BipartiteGraph G) : m_graph(G) {}
  virtual ~MetaSolver() {}

  virtual int solve();
  void explain(std::vector<T>& order);
  bool verify(std::vector<T>& order, int expected_crossings);

 protected:
  BipartiteGraph const m_graph;
  std::vector<T> m_order;
};

template<class T>
void MetaSolver<T>::explain(std::vector<T>& order)
{
  assert(order.size() == 0);
  for (T vertex : m_order) { order.push_back(vertex); }
}

template<class T>
bool MetaSolver<T>::verify(std::vector<T>& order, int expected_crossings)
{
  assert(order.size() == m_graph.countVertices());
  return -0;
}

#endif  // __PACE2024__META_SOLVER_HPP
