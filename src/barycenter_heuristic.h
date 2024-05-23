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

namespace banana {
namespace solver {
namespace heuristic {
namespace barycenter {

/**
 * Implements the Barycenter Heuristic
 *
 * In short, the algorithm orders the vertices on layer B by the mean of their
 * neighbors indexes. If two vertices share the same mean, it preserves their
 * original arrangement. This can be done in polynomial time BIGO( max(M,
 * |B|log|B|) ), where M is the number of edges in the graph.
 *
 * The Barycenter Heuristic was the first popular approximation algorithm
 * developed for the OSCM problem, credited to Sugiyama & Tagawa (1981). Its
 * theorical bounds and performance are well documented in the literature, e.g.
 * Makinen(1990), Eades & Wormald (1994), Junger & Mutzel (1997), Li & Stallmann
 * (2001).
 *
 * The approximation ratio is BIGTHETA(min(sqrt(n), n/d)), where d is the
 * minimum degree of any vertex, or BIGO(D-1), where D is the maximum degree of
 * a vertex in the permutable layer.
 *
 * In practice, it boasts decent results on sparse graphs, whereas other
 * heuristics are either too inaccurate or too slow.
 */
class BarycenterHeuristic : public ApproximationRoutine
{
public:
  BarycenterHeuristic(SubProblem instance);
  ~BarycenterHeuristic() override = default;
  int solve() override;

private:
  std::vector<std::pair<int, int>> m_neighborhoodInfo;
  std::pair<int, int> getNeighborhoodInfo(int node);
};

} // namespace barycenter
} // namespace heuristic
} // namespace solver
} // namespace banana

#endif // __PACE_2024__BARYCENTER_HEURISTIC_H
