/******************************************************************************
 * Top contributors (to current version):
 * Laila
 *
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 *
 * Subdivided Wheel Recognizer
 */

#include "subdivided_wheel_recognizer.h"

namespace banana {
namespace recognizer {

/**
 * Basically it deletes all the paths from the center to 3-degree-vertices and
 * checks if the resulted graph is a cicle
 */

SubdividedWheelRecognizer::SubdividedWheelRecognizer(graph::Graph &graph)
    : Recognizer(graph)
{}

bool SubdividedWheelRecognizer::check()
{

  unsigned n = m_graph.countVertices();
  unsigned count_degree_3 = 0;
  int center = -1;
  int degree_3 = -1;

  std::vector<unsigned> degree = m_graph.degree();

  for (unsigned i = 0; i < n; i++)
  {
    if (degree[i] == 3)
    {
      count_degree_3++;
      degree_3 = i;
    }

    else if (degree[i] > 3 and center != -1)
    {
      return false;
    }
    else if (degree[i] <= 1)
    {
      return false;
    }
    else if (degree[i] > 3)
    {
      center = i;
    }
  }

  if (center == -1 and count_degree_3 == 4)
  {
    center = degree_3;
    count_degree_3--;
  }

  if (count_degree_3 != degree[center])
  {
    return false;
  }
  if (center == -1)
  {
    return false;
  }

  std::vector<bool> visited(n, false);
  std::vector<int> stack(n);
  stack.push_back(center);

  while (!stack.empty())
  {
    unsigned u = stack.back();
    stack.pop_back();

    if (!visited[u])
    {
      visited[u] = true;
      for (int v : m_graph.neighborhood(u))
      {
        if (!visited[v] and degree[v] == 2)
        {
          stack.push_back(v);
        }
      }
    }
  }

  for (unsigned i = 0; i < n; i++)
  {
    if (visited[i])
    {
      for (int u : m_graph.neighborhood(i))
      {
        if (!visited[u])
        {
          degree[u]--;
        }
      }
    }
  }

  for (unsigned i = 0; i < n; i++)
  {
    if (!visited[i] and degree[i] > 2)
    {
      return false;
    }
  }

  stack.push_back(degree_3);
  while (!stack.empty())
  {
    unsigned u = stack.back();
    stack.pop_back();

    if (!visited[u])
    {
      visited[u] = true;
      for (int v : m_graph.neighborhood(u))
      {
        if (!visited[v])
        {
          stack.push_back(v);
        }
      }
    }
  }

  for (unsigned i = 0; i < n; i++)
  {
    if (!visited[i])
    {
      return false;
    }
  }
  return true;
}

} // namespace recognizer
} // namespace banana
