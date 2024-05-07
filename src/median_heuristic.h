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
 * Implementation of the Median Heuristic for OSCM
 */

#ifndef __PACE2024__MEDIAN_HEURISTIC_H
#define __PACE2024__MEDIAN_HEURISTIC_H

#include "approximation_routine.h"
#include <vector>
#include <utility>

namespace banana {
namespace solver {
namespace heuristic {
namespace median {

/**
 * Implements the Median Heuristic
 */
class MedianHeuristic : public ApproximationRoutine
{
public:
  MedianHeuristic(graph::BipartiteGraph graph);
  ~MedianHeuristic() override = default;
  int solve() override;

private:
  std::vector<int> *couting;
  int median(std::vector<int> &neighbors);
};

} // namespace median
} // namespace heuristic
} // namespace solver
} // namespace banana

#endif // __PACE_2024__MEDIAN_HEURISTIC_H