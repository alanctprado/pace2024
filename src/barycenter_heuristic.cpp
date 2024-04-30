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
namespace approx_routine{
namespace barycenter{
  
BarycenterHeuristic::BarycenterHeuristic(graph::BipartiteGraph graph)
    : ApproximationRoutine(graph)
{
  int n = graph.CountVerticesA();
  int offset = graph.CountVerticesB();
  
  for (int i = 0; i < offset; ++i){
    neighborhood_info.push_back(getNeighborhoodInfo(n+i));
  }
}

int BarycenterHeuristic::runRoutine()
{
  int n = graph->countVerticesA();
  int offset = graph->countVerticesB();
  
  std::vector<int> blayer;
  for (int i = 0; i < offset; ++i) blayer.push_back(i);

  std::sort(blayer.begin(), blayer.end(), compareNodes());

  for (int i = 0; i < offset; ++i) blayer[i] += n;

  return numberOfCrossings(blayer);
}

pair<int,int> BarycenterHeuristic::getNeighborhoodInfo(int node){
  std::vector<int> neighbors = m_graph->neighborhood(node);

  int neighborhood_sum = 0;
  for (auto v : neighbors) neighbordhood_sum += v;

  int neighbordhood_size = neighbors.size();

  std::pair<int, int> info = {neighborhood_sum, neighbordhood_size};
  return info;
}

bool BarycenterHeuristic::compareNodes(int &node1, int &node2){
  long long v1 = (1ll * neighborhood_info[node1].f * neighborhood_info[node2].s);
  long long v2 = (1ll * neighborhood_info[node2].f * neighborhood_info[node1].s);

  return (v1 < v2);
}


} // namespace barycenter
} // namespace approx_routine
} // namespace banana