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

namespace banana {
namespace graph {

BipartiteGraph::BipartiteGraph(int n0, int n1) : Graph(n0 + n1)
{
  for (unsigned i = 0; i < (unsigned) n0; i++)
  {
    m_partA.push_back(i);
  }
  for (unsigned i = n0; i < (unsigned) (n0 + n1); i++)
  {
    m_partB.push_back(i);
  }
}

BipartiteGraph::BipartiteGraph(const BipartiteGraph& H) : Graph(H)
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
  return (unsigned) m_partA.size();
}

unsigned BipartiteGraph::countVerticesB() const
{
  return (unsigned) m_partB.size();
}

std::vector<std::vector<int>> BipartiteGraph::buildCrossingMatrix() const
{
  unsigned n_b = countVerticesB();
  std::vector<std::vector<int>> crossing_matrix(n_b, std::vector<int>(n_b, 0));
  for (int i = 0; i < n_b; i++)
  {
    for (int j = 0; j < n_b; j++)
    {
      if (i == j) { continue; }
      int count_crossings = 0;
      for (int a_i : neighborhood(m_partB[i]))
      {
        for (int a_j : neighborhood(m_partB[j]))
        {
          if (a_j < a_i) { count_crossings++; }
        }
      }
      crossing_matrix[i][j] = count_crossings;
    }
  }
  return crossing_matrix;
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
