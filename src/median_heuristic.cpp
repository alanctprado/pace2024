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
#include <algorithm>

namespace banana {
namespace solver {
namespace heuristic {
namespace median {

MedianHeuristic::MedianHeuristic(graph::BipartiteGraph graph)
    : ApproximationRoutine(graph)
{
}

int MedianHeuristic::median(std::vector<int>& neighbors)
{   
    if(neighbors.size()==0) return 0;

    auto m = neighbors.begin() + neighbors.size() / 2;
    std::nth_element(neighbors.begin(), m, neighbors.end());
    
    return neighbors[neighbors.size()/2];
}

int MedianHeuristic::solve()
{
  int n0 = m_graph.countVerticesA();
  int n1 = m_graph.countVerticesB();
  std::vector<std::vector<int>> med_layer(n0);

  for (int i = n0; i < n1 + n0; i++)
  {
    std::vector<int> neighborhood = m_graph.neighborhood(i);
    int med = MedianHeuristic::median(neighborhood);
    med_layer[med].push_back(i);
  }

  std::vector<int> b_layer;
  for (int i = 0; i < n0; i++)
  {
    for (int j = 0; j < med_layer[i].size(); j++)
    {
      b_layer.push_back(med_layer[i][j]);
    }
  }

  return numberOfCrossings(b_layer);
}

} // namespace median
} // namespace heuristic
} // namespace solver
} // namespace banana
