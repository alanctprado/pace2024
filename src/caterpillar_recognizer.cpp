/******************************************************************************
 * Top contributors (to current version):
 *  Laila
 *
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 *
 * Caterpillar Recognizer
 */

#include "caterpillar_recognizer.h"

namespace banana {
namespace recognizer {

/*
    Basically it deletes the 1-degree-vertices and sees if the resulted graph is
   a path
*/

CaterpillarRecognizer::CaterpillarRecognizer(graph::Graph &graph)
    : Recognizer(graph)
{}

bool CaterpillarRecognizer::check()
{

  bool is_tree = m_graph.isTree();
  if (!is_tree)
  {
    return false;
  }

  unsigned n = m_graph.countVertices();
  std::vector<int> visited(n);
  std::vector<unsigned> degree = m_graph.degree();

  for (unsigned i = 0; i < n; i++)
  {
    if (degree[i] == 1)
    {
      visited[i] = 1;
    }
  }

  for (unsigned i = 0; i < n; i++)
  {
    if (visited[i])
    {
      degree[i]--;
      for (auto u : m_graph.neighborhood(i))
      {
        if (!visited[u])
        {
          degree[u]--;
        }
      }
    }
  }

  unsigned count_degree_1 = 0;
  for (int i = 0; i < n; i++)
  {
    if (!visited[i])
    {
      if (degree[i] == 1)
      {
        count_degree_1++;
      }
      else if (degree[i] > 2)
      {
        return false;
      }
    }
  }

  if (count_degree_1 > 2)
  {
    return false;
  }
  return true;
}

} // namespace recognizer
} // namespace banana
