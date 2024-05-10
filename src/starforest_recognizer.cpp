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
 * Star Forest Recognizer
 */

#include "starforest_recognizer.h"

namespace banana {
namespace recognizer {

/**
 * Basically it first checks if it is a forest and than checks the
 * vertices-degrees to see if it is a star-forest
 */

StarForestRecognizer::StarForestRecognizer(graph::Graph &graph)
    : Recognizer(graph)
{}

bool StarForestRecognizer::check()
{
  bool is_forest = m_graph.isForest();
  if (!is_forest)
  {
    return false;
  }
  unsigned n = m_graph.countVertices();
  std::vector<unsigned> degree = m_graph.degree();
  std::vector<bool> deleted(n);

  for (unsigned i = 0; i < n; i++)
  {
    if (degree[i] == 1)
    {
      deleted[i] = true;
    }
  }

  for (int i = 0; i < n; i++)
  {
    if (deleted[i])
    {
      for (int u : m_graph.neighborhood(i))
      {
        degree[u]--;
      }
    }
  }

  for (unsigned i = 0; i < n; i++)
  {
    if (!deleted[i] and degree[i] > 0)
    {
      return false;
    }
  }

  return true;
}

} // namespace recognizer
} // namespace banana
