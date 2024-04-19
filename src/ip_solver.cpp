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
#include "../lp_solve_5.5/lp_lib.h"

#include <iostream>
#include <algorithm>

namespace banana {
namespace solver {
namespace ip {

IntegerProgrammingSolver::IntegerProgrammingSolver(graph::BipartiteGraph graph)
    : MetaSolver<int>(graph)
{}

int IntegerProgrammingSolver::solve()
{
  std::vector<std::vector<int>> cm = m_graph.buildCrossingMatrix();

  int n = m_graph.countVerticesB();
  int offset = m_graph.countVerticesA();

  lprec* lp;
  lp = make_lp(0, n * n);
  set_verbose(lp, SEVERE);

  set_minim(lp);

  double* c = (double*) malloc((n * n + 1) * sizeof(double));

  c[0] = 0;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      c[i * n + j + 1] = cm[i][j];
    }
  }
  set_obj_fn(lp, c);

  memset(c, 0, (n * n + 1) * sizeof(double));

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      for (int k = 0; k < n; k++) {
        if (i == j or i == k or j == k) continue;
        c[i * n + j + 1] = c[j * n + k + 1] = 1;
        c[i * n + k + 1] = -1;
        add_constraint(lp, c, LE, 1);
        c[i * n + j + 1] = c[j * n + k + 1] = c[i * n + k + 1] = 0;
      }
    }
  }

  for (int i = 0; i < n; i++) {
    for (int j = i + 1; j < n; j++) {
      c[i * n + j + 1] = c[j * n + i + 1] = 1;
      add_constraint(lp, c, EQ, 1);
      c[i * n + j + 1] = c[j * n + i + 1] = 0;
    }
  }

  free(c);

  for (int i = 1; i <= n * n; i++)
    set_binary(lp, i, TRUE);

  if (::solve(lp)) {
    std::cout << "Couldn't find an optimal solution :-(" << std::endl;
    exit(1);
  }

  double* vars = (double*) malloc((n * n) * sizeof(double));
  get_variables(lp, vars);
  std::vector<std::pair<int, int>> sol;
  for (int i = 0; i < n; i++) {
    int comes_first = 0;
    for (int j = 0; j < n; j++) {
       if (i == j) continue;
       comes_first += vars[i * n + j];
    }
    sol.push_back({comes_first, i});
  }

  std::sort(sol.begin(), sol.end());
  for (int i = n - 1; i >=0; i--)
    m_order.push_back(sol[i].second + offset + 1);

  double z = get_objective(lp);
  delete_lp(lp);

  return (int) z;
}

} // namespace ip
} // namespace solver
} // namespace banana
