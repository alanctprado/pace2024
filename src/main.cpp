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
#include "environment.h"
#include "meta_solver.h"
#include "utils.h"
#include "oracle.h"
#include "dp_solver.h"
#include <memory>

/** Remove assertions before submitting!! */

int main(int argc, char *argv[])
{
  banana::Environment env;
  env.setOptions(argc, argv);
  std::unique_ptr<banana::graph::BipartiteGraph> input_graph;
  banana::utils::readBipartiteGraph(input_graph);
  env.initOracle(*input_graph);

  banana::solver::Oracle::SubProblem subProblem;
  for (int i = 0; i < input_graph->countVerticesB(); i++)
    subProblem.emplace_back(i, 1);
  banana::solver::MetaSolver* bananasso = new banana::solver::dp::DPSolver(subProblem);
  int ans2 = bananasso->solve();
  std::vector<banana::solver::Oracle::Vertex> ans;
  bananasso->explain(ans);
  for (auto[v, f] : ans)
    std::cout << v + input_graph->countVerticesA() + 1 << "\n";
  
  exit(0);
}
