/******************************************************************************
 * Top contributors (to current version):
 *   Kaio Vieira, Gabriel Ubiratan
 *
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 *
 * Data Structure to store and index information about the instance being solved
 */
#ifndef __PACE_2024_SUBPROBLEM_HPP
#define __PACE_2024_SUBPROBLEM_HPP

#include "bipartite_graph.h"
#include "environment.h"
#include "oracle.h"


namespace banana {
namespace solver {

/**
 * TODO: Explain how we're representing instances (subproblems) of the graph.
 */
typedef std::vector<std::pair<int, Oracle::F>> SubProblem;

} // namespace solver
} // namespace banana

#endif // __PACE_2024_SUBPROBLEM_HPP
