/******************************************************************************
 * Top contributors (to current version):
 *   Alan Prado
 *
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 *
 * Integer programming solver. TO-DO.
 */

#include "ip_solver.h"
#include "meta_solver.h"

IntegerProgrammingSolver::IntegerProgrammingSolver(BipartiteGraph graph)
    : MetaSolver<int>(graph)
{}

int IntegerProgrammingSolver::solve()
{
  return 0;
}
