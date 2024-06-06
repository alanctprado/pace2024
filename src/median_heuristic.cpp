/******************************************************************************
 * Top contributors (to current version):
 *   Gabriel Vieira
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

#include "median_heuristic.h"
#include "environment.h"
#include <algorithm>

namespace banana {
namespace solver {
namespace heuristic {
namespace median {

MedianHeuristic::MedianHeuristic(Oracle::SubProblem instance)
    : ApproximationRoutine(instance)
{}

int MedianHeuristic::median(std::vector<int> &neighbors)
{
  if (neighbors.size() == 0)
    return 0;

  auto m = neighbors.begin() + neighbors.size() / 2;
  std::nth_element(neighbors.begin(), m, neighbors.end());

  return neighbors[neighbors.size() / 2];
}

int MedianHeuristic::solve()
{
  const auto& m_oracle = Environment::oracle();
  int n0 = m_oracle.countVerticesA();
  std::vector<std::vector<Oracle::Vertex>> med_layer(n0);

  for (auto [i, frac] : m_instance)
  {
    std::vector<int> neighborhood = m_oracle.neighborhood(i);
    int med = MedianHeuristic::median(neighborhood);
    med_layer[med].push_back({i, frac});
  }

  std::vector<Oracle::Vertex> b_layer;
  for (int i = 0; i < n0; i++)
  {
    for (int j = 0; j < med_layer[i].size(); j++)
    {
      b_layer.push_back(med_layer[i][j]);
    }
  }

  return m_oracle.numberOfCrossings(b_layer);
}

} // namespace median
} // namespace heuristic
} // namespace solver
} // namespace banana
