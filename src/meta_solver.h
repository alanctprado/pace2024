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

#include <algorithm>
#include <cassert>
#include <vector>

namespace banana {
namespace solver {

/**
 * Abstract solver class
 */
template <class T, class U> class MetaSolver
{
public:
  MetaSolver(T G) : m_graph(G) {}
  virtual ~MetaSolver() {}

  virtual int solve() = 0;
  void explain(std::vector<U> &order);
  bool verify(const std::vector<U> &order, int expected_crossings) const;

protected:
  int numberOfCrossings(const std::vector<U> &order) const;

  T const m_graph;
  std::vector<U> m_order;
};

template <class T, class U>
void MetaSolver<T,U>::explain(std::vector<U> &order)
{
  assert(order.size() == 0);
  for (U vertex : m_order)
  {
    order.push_back(vertex);
  }
}

template <class T, class U>
int MetaSolver<T,U>::numberOfCrossings(const std::vector<U> &order) const
{
  int nA = m_graph.countVerticesA();
  int nB = m_graph.countVerticesB();

  std::vector<int> position(nB);
  for (int i = 0; i < order.size(); i++)
  {
    position[order[i] - nA] = i;
  }

  auto edges = m_graph.edges();
  std::sort(edges.begin(), edges.end(), [&](auto edge1, auto edge2) {
    auto [a1, b1] = edge1;
    auto [a2, b2] = edge2;
    if (a1 > b1)
      std::swap(a1, b1);
    if (a2 > b2)
      std::swap(a2, b2);

    if (b1 == b2)
      return a1 < a2;
    return position[b1 - nA] < position[b2 - nA];
  });

  int crossings = 0;
  library::FenwickTree<int> tree(nA);  // shouldn't it be <U>? @mvkaio

  for (int l = 0, r = 0; l < (int)edges.size(); l = r)
  {
    while (r < (int)edges.size() && edges[l].second == edges[r].second)
      r++;

    for (int i = l; i < r; i++)
    {
      auto [v_a, v_b] = edges[i];
      if (v_a > v_b)
        std::swap(v_a, v_b);

      crossings += tree.suffixQuery(v_a + 1);
    }

    for (int i = l; i < r; i++)
    {
      auto [v_a, v_b] = edges[i];
      if (v_a > v_b)
        std::swap(v_a, v_b);

      tree.update(v_a, +1);
    }
  }

  return crossings;
}

template <class T, class U>
bool MetaSolver<T,U>::verify(const std::vector<U> &order,
                             int expected_crossings) const
{
  assert(order.size() == m_graph.countVerticesB());
  return numberOfCrossings(order) == expected_crossings;
}

} // namespace solver
} // namespace banana

#endif // __PACE2024__META_SOLVER_HPP
