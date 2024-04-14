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

unsigned BipartiteGraph::countVerticesA()
{
  return (unsigned) m_partA.size();
}

unsigned BipartiteGraph::countVerticesB()
{
  return (unsigned) m_partB.size();
}

std::vector<std::vector<int>> BipartiteGraph::buildCrossingMatrix()
{
  unsigned n_b = countVerticesB();
  std::vector<std::vector<int>> crossing_matrix(n_b, std::vector<int>(n_b, 0));
  for (int i = 0; i < n_b; i++)
  {
    for (int j = i + 1; j < n_b; j++)
    {
      int count_crossings = 0;
      for (int a_i : neighborhood(m_partB[i]))
      {
        for (int a_j : neighborhood(m_partB[j]))
        {
          if (a_j < a_i) { count_crossings++; }
        }
      }
      crossing_matrix[i][j] = crossing_matrix[j][i] = count_crossings;
    }
  }
  return crossing_matrix;
}
