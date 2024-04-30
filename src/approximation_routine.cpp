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

#include "approximation_routine.h"


namespace banana{
namespace approx_routine{
  
  ApproximationRoutine::ApproximationRoutine(graph::BipartiteGraph graph) : 
      m_graph(graph)
  {}

} // namespace approx_routine
} // namespace banana