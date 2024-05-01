/******************************************************************************
 * Top contributors (to current version):
 *   Matheus Prates
 *
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 *
 * Implementation of the Barycenter Heuristic for OSCM
 */

#include "barycenter_heuristic.h"
#include <algorithm>

namespace banana {
namespace solver {
namespace heuristic {
namespace barycenter {

BarycenterHeuristic::BarycenterHeuristic(graph::BipartiteGraph graph)
    : ApproximationRoutine(graph)
{
  int n = graph.countVerticesB();
  int offset = graph.countVerticesA();

  for (int i = 0; i < n; ++i)
  {
    m_neighborhoodInfo.push_back(getNeighborhoodInfo(i + offset));
  }
}

/**
 * Sorts vertices in layer B by the mean of their neighbors.
 *
 * To avoid working with non-integer values, the corresponding mean for each
 * node is represented as a fraction by the pair (sum of neighbors, number of
 * neighbors)
 */
int BarycenterHeuristic::solve()
{
  int n = m_graph.countVerticesB();
  int offset = m_graph.countVerticesA();

  std::vector<int> b_layer;
  for (int i = 0; i < n; ++i)
  {
    b_layer.push_back(i);
  }

  std::sort(b_layer.begin(), b_layer.end(), [&](int node1, int node2) {
    long long v1 = (1ll * m_neighborhoodInfo[node1].first *
                    m_neighborhoodInfo[node2].second);
    long long v2 = (1ll * m_neighborhoodInfo[node2].first *
                    m_neighborhoodInfo[node1].second);
    return (v1 < v2);
  });

  for (int i = 0; i < n; ++i)
  {
    b_layer[i] += offset;
  }

  return numberOfCrossings(b_layer);
}

/**
 * Calculates the pair (sum of neighbors indexes, number of neighbors) for a
 * given node
 */
std::pair<int, int> BarycenterHeuristic::getNeighborhoodInfo(int node)
{
  std::vector<int> neighbors = m_graph.neighborhood(node);

  int neighborhood_sum = 0;
  for (auto v : neighbors)
    neighborhood_sum += v;

  int neighbordhood_size = neighbors.size();

  std::pair<int, int> info = {neighborhood_sum, neighbordhood_size};
  return info;
}

} // namespace barycenter
} // namespace heuristic
} // namespace solver
} // namespace banana
