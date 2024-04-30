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

namespace banana{
namespace solver{
namespace approx_routine{
namespace barycenter{
  
BarycenterHeuristic::BarycenterHeuristic(graph::BipartiteGraph graph)
    : ApproximationRoutine(graph)
{
  int n = graph.countVerticesA();
  int offset = graph.countVerticesB();
  
  for (int i = 0; i < offset; ++i){
    neighborhood_info.push_back(getNeighborhoodInfo(n+i));
  }
}

int BarycenterHeuristic::solve()
{
  return solveBarycenterHeuristic();
}

int BarycenterHeuristic::solveBarycenterHeuristic(){
  int n = m_graph.countVerticesA();
  int offset = m_graph.countVerticesB();
  
  std::vector<int> blayer;
  for (int i = 0; i < offset; ++i) blayer.push_back(i);

  std::sort(blayer.begin(), blayer.end(), [&](int node1, int node2) {
        long long v1 = (1ll * neighborhood_info[node1].first * neighborhood_info[node2].second);
        long long v2 = (1ll * neighborhood_info[node2].first * neighborhood_info[node1].second);
        return (v1 < v2);
    });

  for (int i = 0; i < offset; ++i) blayer[i] += n;

  return numberOfCrossings(blayer);
}

std::pair<int,int> BarycenterHeuristic::getNeighborhoodInfo(int node){
  std::vector<int> neighbors = m_graph.neighborhood(node);

  int neighborhood_sum = 0;
  for (auto v : neighbors) neighborhood_sum += v;

  int neighbordhood_size = neighbors.size();

  std::pair<int, int> info = {neighborhood_sum, neighbordhood_size};
  return info;
}

} // namespace barycenter
} // namespace approx_routine
} // namespace solver
} // namespace banana