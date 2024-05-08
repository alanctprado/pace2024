/******************************************************************************
 * Top contributors (to current version):
 *   Heitor Leite
 *
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 *
 * Crossing matrix.
 */

#include "crossing_matrix.h"
#include "bipartite_graph.h"
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace banana {
namespace crossing {

CrossingMatrix::CrossingMatrix(graph::BipartiteGraph graph) {
  /* Computes the naive interval system */
  std::vector<std::vector<int>> open(graph.countVerticesA()+1);
  std::vector<std::vector<int>> close(graph.countVerticesA()+1);
  std::unordered_map<int, int> right;

  for (int v : graph.getB())
  {
    int l = -1, r = -1;
    for (int u : graph.neighborhood(v))
    {
      if (l == -1 || u < l) l = u;
      if (r == -1 || u > r) r = u;
    }
    right[v] = r;

    if (l == -1) continue;
    open[l].push_back(v);
    close[r].push_back(v);
  }

  /* NOTE: The paper users a doubly linked list, maybe we should consider that?
   *
   * NOTE: The paper only consider active vertices whose interval that
   * strictly contain (l < x < r) the current vertex, but that seemed to produce
   * wrong results. (l <= x <= r) works, but seems to compute crossing numbers
   * for some non-orientable pairs too. Further research is needed. */
  std::unordered_set<int> active;

  /* First scan trough A: initializes counters for all orientable pairs
   *
   * We can probably only do one scan for now, this first one is used only to
   * limit computation in the decision version of the problem (if a counter is
   * initialized more than 2k times, the instance is infeasible)
   */
  for (int a : graph.getA())
  {
    for (int b : open[a]) active.insert(b);

    for (int u : graph.neighborhood(a))
      for (int v : active)
        if (u != v) m_map[u][v] = 0;
    
    for (int b : close[a]) active.erase(b);
  }

  std::unordered_map<int, int> d_less, d_leq;

  /* Second scan: computes crossing numbers for orientable pairs */
  for (int a : graph.getA())
  {
    for (int b : graph.neighborhood(a)) d_leq[b]++;
    for (int b : open[a]) active.insert(b);

    for (int u : graph.neighborhood(a))
      for (int v : active)
        if (u != v) m_map[u][v] += d_less[v];

    for (int u : active)
      for (int v : graph.neighborhood(a))
      {
        if (u == v || right[v] != a) continue;
        m_map[u][v] += graph.degree(v) * (graph.degree(u) - d_leq[u]);
      }

    for (int b : close[a]) active.erase(b);
    for (int b : graph.neighborhood(a)) d_less[b]++;
  }
}

/* TODO: decide how to handle forced and free pairs */
int CrossingMatrix::CrossingMatrix::operator()(int u, int v) const
{
  /* NOTE: we want .at() here because of const */
  if (m_map.find(u) == m_map.end()) return -1;
  auto x = m_map.at(u);
  if (x.find(v) == x.end()) return -1;
  return x.at(v);
};

} // namespace crossing
} // namespace banana
