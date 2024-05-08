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
 * Bipartite graph class.
 */

#include "bipartite_graph.h"
#include "graph.h"
#include <algorithm>

namespace banana {
namespace graph {

BipartiteGraph::BipartiteGraph(int n0, int n1) : LGraph(n0 + n1)
{
  for (unsigned i = 0; i < (unsigned)n0; i++)
  {
    m_partA.push_back(i);
  }
  for (unsigned i = n0; i < (unsigned)(n0 + n1); i++)
  {
    m_partB.push_back(i);
  }
}

BipartiteGraph::BipartiteGraph(const BipartiteGraph &H) : LGraph(H)
{
  for (int a : H.m_partA)
  {
    m_partA.push_back(a);
  }
  for (int b : H.m_partB)
  {
    m_partB.push_back(b);
  }
}

unsigned BipartiteGraph::countVerticesA() const
{
  return (unsigned)m_partA.size();
}

unsigned BipartiteGraph::countVerticesB() const
{
  return (unsigned)m_partB.size();
}

std::vector<std::vector<int>> BipartiteGraph::buildCrossingMatrix() const
{
  unsigned n_b = countVerticesB();
  std::vector<std::vector<int>> crossing_matrix(n_b, std::vector<int>(n_b, 0));
  for (int i = 0; i < n_b; i++)
  {
    for (int j = 0; j < n_b; j++)
    {
      if (i == j)
      {
        continue;
      }
      int count_crossings = 0;
      for (int a_i : neighborhood(m_partB[i]))
      {
        for (int a_j : neighborhood(m_partB[j]))
        {
          if (a_j < a_i)
          {
            count_crossings++;
          }
        }
      }
      crossing_matrix[i][j] = count_crossings;
    }
  }
  return crossing_matrix;
}

std::vector<std::pair<int, int>> BipartiteGraph ::intervalPairs() const
{
  std::vector<std::tuple<int, bool, int>> endpoints;

  for (int v : m_partB)
  {
    if (degree(v) == 0)
      continue;
    int mn = neighborhood(v)[0];
    int mx = neighborhood(v)[0];
    for (int u : neighborhood(v))
    {
      if (u < mn)
      {
        mn = u;
      }
      if (u > mx)
      {
        mx = u;
      }
    }
    endpoints.push_back({mn, 1, v});
    endpoints.push_back({mx, 0, v});
  }
  std::sort(endpoints.begin(), endpoints.end());
  std::reverse(endpoints.begin(), endpoints.end());

  std::vector<int> prefix;
  std::vector<std::pair<int, int>> pairs;

  while (!endpoints.empty())
  {
    auto [v, b, i] = endpoints.back();
    endpoints.pop_back();
    if (!b)
    {
      prefix.push_back(i);
    }
    else
    {
      for (int u : prefix)
      {
        pairs.push_back({u, i});
      }
    }
  }
  return pairs;
}

std::vector<int> BipartiteGraph::getA() const
{
  return m_partA;
}

std::vector<int> BipartiteGraph::getB() const
{
  return m_partB;
}

} // namespace graph
} // namespace banana
