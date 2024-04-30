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
 * Base solver for the one-sided crossing minimization problem
 */

#ifndef __PACE2024__APPROXIMATION_ROUTINE_H
#define __PACE2024__APPROXIMATION_ROUTINE_H

#include "bipartite_graph.h"

namespace banana{
namespace approx_routine{

class ApproximationRoutine
{
  public:
    ApproximationRoutine(graph::BipartiteGraph graph);
    ~ApproximationRoutine() = default;
    int runRoutine() = 0;

  protected:
    const graph::BipartiteGraph m_graph;
}

} // namespace approx_routine
} // namespace banana


#endif // __PACE_2024__APPROXIMATION_ROUTINE_H


