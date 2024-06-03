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
#include "environment.h"
#include "oracle.h"
#include <algorithm>

namespace banana {
namespace solver {
namespace heuristic {
namespace barycenter {

BarycenterHeuristic::BarycenterHeuristic(Oracle::SubProblem instance) : ApproximationRoutine(instance)
{
  for (auto [i, _] : instance)
  {
    m_neighborhoodInfo.push_back(getNeighborhoodInfo(i));
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
  int n = m_neighborhoodInfo.size();

  Oracle::SubProblem b_layer = m_instance;

  std::sort(b_layer.begin(), b_layer.end(), [&](auto node1, auto node2) {
    auto [i, _1] = node1;
    auto [j, _2] = node2;
    long long v1 = (1ll * m_neighborhoodInfo[i].first *
                    m_neighborhoodInfo[j].second);
    long long v2 = (1ll * m_neighborhoodInfo[j].first *
                    m_neighborhoodInfo[i].second);
    return (v1 < v2);
  });

  return Environment::oracle().numberOfCrossings(b_layer);
}

/**
 * Calculates the pair (sum of neighbors indexes, number of neighbors) for a
 * given node
 */
std::pair<int, int> BarycenterHeuristic::getNeighborhoodInfo(int node)
{
  auto &_oracle = Environment::oracle();
  std::vector<int> neighbors = _oracle.neighborhood(node);

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
