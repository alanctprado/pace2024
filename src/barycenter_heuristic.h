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

#ifndef __PACE2024__BARYCENTER_HEURISTIC_H
#define __PACE2024__BARYCENTER_HEURISTIC_H

#include "approximation_routine.h"
#include <vector>
#include <pair>

namespace banana{
namespace approx_routine{
namespace barycenter{

class BarycenterHeuristic : ApproximationRoutine
{
public:
  BarycenterHeuristic(graph::BipartiteGraph graph);
  ~BarycenterHeuristic() override = default;
  int runRoutine() override;

private:
  std::vector<pair<int,int>> neighborhood_info; //sum of neighbors, number of neighbors
  std::pair<int,int> getNeighborhoodInfo(int node);
  bool compareNodes(int node1, int node2);
}; 

} // namespace barycenter
} // namespace approx_routine
} // namespace banana


#endif // __PACE_2024__BARYCENTER_HEURISTIC_H


