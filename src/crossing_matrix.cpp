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
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <array>
#include <unordered_set>
#include <unordered_map>
#include <iostream>

namespace banana {
namespace crossing {

std::array<std::unordered_map<int, int>, 2>
CrossingMatrix::getIntervals(const graph::BipartiteGraph &graph)
{

  std::unordered_map<int, int> l, r;

  for (int v : graph.getB())
  {
    l[v] = -1, r[v] = -1;
    for (int u : graph.neighborhood(v))
    {
      if (l[v] == -1 || u < l[v])
        l[v] = u;
      if (r[v] == -1 || u > r[v])
        r[v] = u;
    }
  }

  return {l, r};
}

CrossingMatrix::CrossingMatrix(graph::BipartiteGraph graph)
{
  /* Computes the naive interval system */
  std::vector<std::vector<int>> open(graph.countVerticesA() + 1);
  std::vector<std::vector<int>> close(graph.countVerticesA() + 1);
  std::vector<std::vector<int>> openclose(graph.countVerticesA() + 1);

  auto intervals = getIntervals(graph);
  std::unordered_map<int, int> l, r;
  l = intervals[0], r = intervals[1];

  for (int v : graph.getB())
  {
    if (l[v] == -1)
      continue;
    if (l[v] == r[v])
      openclose[l[v]].push_back(v);
    else
    {
      open[l[v]].push_back(v);
      close[r[v]].push_back(v);
    }
  }

  /* NOTE: The paper users a doubly linked list, maybe we should consider that?
   *
   * NOTE: The paper only consider active vertices whose interval that
   * strictly contain (l < x < r) the current vertex, but that seemed to produce
   * wrong results. (l <= x <= r) works, but seems to compute crossing numbers
   * for some non-orientable pairs too. Instead, we implemented an alternative
   * similar algorithm. */
  std::unordered_set<int> active;

  /* TODO: use vectors */
  std::unordered_map<int, int> d_less, d_leq;

  for (int a : graph.getA())
  {
    for (int b : graph.neighborhood(a))
      d_leq[b]++;

    for (int u : graph.neighborhood(a))
    {
      if (l[u] == a)
        continue;
      for (int v : close[a])
        if (u != v)
          m_map[{u, v}] += d_less[v];
    }

    for (int b : close[a])
      active.erase(b);

    for (int u : graph.neighborhood(a))
      for (int v : active)
        if (u != v)
          m_map[{u, v}] += d_less[v];

    for (int b : open[a])
      active.insert(b);
    for (int b : graph.neighborhood(a))
      d_less[b]++;
  }

  if (!active.empty())
    throw std::runtime_error("active is not empty");
  active.clear(), d_leq.clear(), d_less.clear();

  for (int a : graph.getA())
  {
    for (int b : graph.neighborhood(a))
      d_leq[b]++;
    for (int b : close[a])
      active.erase(b);

    for (int u : active)
    {
      for (int v : close[a])
        m_map[{u, v}] += graph.degree(v) * (graph.degree(u) - d_leq[u]);

      for (int v : openclose[a])
        m_map[{u, v}] += graph.degree(v) * (graph.degree(u) - d_leq[u]);
    }

    for (int b : open[a])
      active.insert(b);
    for (int b : graph.neighborhood(a))
      d_less[b]++;
  }
}

/* TODO: decide how to handle forced and free pairs */
int CrossingMatrix::CrossingMatrix::operator()(int u, int v) const
{
  /* NOTE: we want .at() here because of const */
  if (m_map.find({u, v}) == m_map.end())
    return -1;
  return m_map.at({u, v});
};

std::vector<std::pair<int, int>> CrossingMatrix::getOrientablePairs()
{
  std::vector<std::pair<int, int>> res;
  for (auto [p, c] : m_map)
    res.push_back(p);
  std::sort(res.begin(), res.end());
  return res;
}

} // namespace crossing
} // namespace banana

