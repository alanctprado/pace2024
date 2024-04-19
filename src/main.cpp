/******************************************************************************
 * Top contributors (to current version):
 *   Alan Prado, Kaio Vieira, Luis Higino
 *
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 *
 * Main.
 */

#include "bipartite_graph.h"
#include "base_solver.h"
#include "utils.h"

#include <memory>
#include <iostream>

int main(int argc, char* argv[])
{
  std::unique_ptr<banana::graph::BipartiteGraph> input_graph;
  banana::utils::readBipartiteGraph(input_graph);
  banana::solver::BaseSolver bananao(*input_graph.get());
  bananao.runBanana();

	exit(0);
}
