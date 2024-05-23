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

#include "approximation_routine.h"

namespace banana {
namespace solver {
namespace heuristic {

ApproximationRoutine::ApproximationRoutine(SubProblem instance)
    : MetaSolver(instance)
{}

} // namespace heuristic
} // namespace solver
} // namespace banana