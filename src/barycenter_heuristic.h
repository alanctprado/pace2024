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
#include <utility>

namespace banana{
namespace solver{
namespace approx_routine{
namespace barycenter{

/**
 * Implements the barycenter heuristic
 */
class BarycenterHeuristic : public ApproximationRoutine
{
public:
  BarycenterHeuristic(graph::BipartiteGraph graph);
  ~BarycenterHeuristic() override = default;
  int solve() override;

private:
  std::vector<std::pair<int,int>> neighborhood_info; //sum of neighbors, number of neighbors
  int solveBarycenterHeuristic();
  std::pair<int,int> getNeighborhoodInfo(int node);
}; 

} // namespace barycenter
} // namespace approx_routine
} // namespace solver
} // namespace banana


#endif // __PACE_2024__BARYCENTER_HEURISTIC_H


