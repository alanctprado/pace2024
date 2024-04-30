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
 * Definition of an abstract approximation routine class
 */

#ifndef __PACE2024__APPROXIMATION_ROUTINE_H
#define __PACE2024__APPROXIMATION_ROUTINE_H

#include "meta_solver.h"

namespace banana{
namespace solver{
namespace approx_routine{

/**
 * Abstract approximation routine class
 */
class ApproximationRoutine : public MetaSolver<int>
{
public:
  ApproximationRoutine(graph::BipartiteGraph graph);
  virtual ~ApproximationRoutine() = default;
};

} // namespace approx_routine
} // namespace solver
} // namespace banana


#endif // __PACE_2024__APPROXIMATION_ROUTINE_H


