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
 *
 * The algorithm sorts the vertices of layer B based on the median of their
 * neighborhood. If two vertices have the same median, their relative
 * positions are preserved. Isolated vertices are positioned at the beginning.
 * Its asymptotic complexity is O(|B| * |A| log|A|).
 *
 * This algorithm has been proven to be 3-approximate. Furthermore, it can be
 * proven that there is no approximation algorithm that decides the position of
 * a vertex based on its neighborhood with an approximation factor less than 3.
 *
 */

class MedianHeuristic : public ApproximationRoutine
{
public:
  MedianHeuristic(Oracle::SubProblem instance);
  ~MedianHeuristic() override = default;
  int solve() override;

private:
  int median(std::vector<int> &neighbors);
};

} // namespace median
} // namespace heuristic
} // namespace solver
} // namespace banana

#endif // __PACE_2024__MEDIAN_HEURISTIC_H
