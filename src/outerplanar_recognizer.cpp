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
 * Oterplanar Recognizer
 */

#include "outerplanar_recognizer.h"
#include <queue>
#include <algorithm>

namespace banana {
namespace recognizer {

/**
 * It recognizes if a biconected component is outerplanar by triangulating the
 * graph and seeing if the resulted graph is maximal outerplanar IMPORTANT: it
 * assumes that the graph is biconnected
 */

OuterplanarRecognizer::OuterplanarRecognizer(graph::Graph &graph)
    : Recognizer(graph)
{}

bool OuterplanarRecognizer::check()
{

  unsigned n = m_graph.countVertices();
  unsigned m = m_graph.countEdges();

  std::vector<std::pair<int, int>> edges = m_graph.edges();
  if (edges.size() > 2 * n - 3)
  {
    return false;
  }

  std::queue<int> degree_2;
  std::vector<unsigned> degree = m_graph.degree();

  for (int i = 0; i < n; i++)
  {
    if (degree[i] == 2)
    {
      degree_2.push(i);
    }
  }

  if (degree_2.size() < 2)
  {
    return false;
  }

  int last_near, last_next;
  std::vector<std::vector<int>> new_adj(n);
  std::vector<int> deleted(n);
  std::vector<std::pair<int, int>> pairs;

  int l = 1;
  while (l <= n - 2)
  {
    int node = degree_2.front();
    degree_2.pop();
    if (degree[node] != 2)
    {
      continue;
    }

    std::vector<int> aux;
    for (auto u : m_graph.neighborhood(node))
    {
      if (!deleted[u])
      {
        aux.push_back(u);
      }
    }

    for (auto u : new_adj[node])
    {
      if (!deleted[u])
      {
        aux.push_back(u);
        if (node <= u)
        {
          edges.push_back({node, u});
        }
        else
        {
          edges.push_back({u, node});
        }
      }
    }

    if (aux[0] > aux[1])
    {
      std::swap(aux[0], aux[1]);
    }
    pairs.push_back({aux[0], aux[1]});

    bool is_adj = 0;
    for (auto u : m_graph.neighborhood(aux[0]))
    {
      if (u == aux[1])
      {
        is_adj = 1;
        break;
      }
    }

    if (!is_adj)
    {
      new_adj[aux[0]].push_back(aux[1]);
      new_adj[aux[1]].push_back(aux[0]);
      degree[aux[0]]++;
      degree[aux[1]]++;
    }

    deleted[node] = true;
    for (auto u : m_graph.neighborhood(node))
    {
      degree[u]--;
      if (degree[u] == 2)
      {
        degree_2.push(u);
      }
    }

    if (degree_2.size() < 2)
    {
      return false;
    }

    last_near = aux[0];
    last_next = aux[1];

    l++;
  }

  edges.push_back({last_near, last_next});

  std::sort(edges.begin(), edges.end());
  std::sort(pairs.begin(), pairs.end());

  int j = 0;
  for (int i = 0; i < edges.size();)
  {
    if (pairs[j] == edges[i])
    {
      j++;
    }
    else
    {
      i++;
    }
  }

  if (j < pairs.size())
  {
    return false;
  }
  else
  {
    return true;
  }
}

} // namespace recognizer
} // namespace banana
