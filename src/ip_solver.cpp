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
 * Integer programming solver.
 */

#include "ip_solver.h"
#include "environment.h"
#include "meta_solver.h"
#include "options.h"
#include "../lp_solve_5.5/lp_lib.h"
#include "subproblem.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace banana {
namespace solver {
namespace ip {

IntegerProgrammingSolver::IntegerProgrammingSolver(SubProblem G)
    : MetaSolver(G)
{
  for (int i = 0; i < m_instance.size(); i++)
  {
    m_weight[m_instance[i].first] = m_instance[i].second;
  }
}

int IntegerProgrammingSolver::solve()
{
  options::HolderIP ip_options = Environment::options().ip;
  switch (ip_options.solverMode)
  {
  case options::IPSolverMode::LPSOLVE:
    if (ip_options.formulation == options::IPFormulation::ONE)
    {
      return solveWithLPSolve1();
    }
    else if (ip_options.formulation == options::IPFormulation::TWO)
    {
      return solveWithLPSolve2();
    }
  default:
    break;
  }
  throw std::runtime_error("Do the L");
}

/**
 * Integer Program 1
 *
 * Variables:
 *   cm:      crossing matrix, cm_{i,j} counts the numbers of crossings that
 *            happen if B_i comes before B_j in the ordering.
 *   x_{i,j}: 0-1 variable that is 1 iff B_i appears before B_j in the ordering
 *
 * Formulation:
 *   minimize \sum_{i,j} cm_{i,j} \cdot x_{i,j}
 *
 *   subject to: x_{i,j} + x_{j,k} - x_{i,k} \leq 1 \forall i \neq j \neq k
 *               (transitivity constraint)
 *
 *               x_{i,j} + x_{j, i} = 1 \forall i < j
 *               (exactly one is true, we actually only need at least one true)
 *
 *               x_{i,j} \in \{0, 1\} \forall i, j
 *               (integer program with 0-1 variables)
 */
int IntegerProgrammingSolver::solveWithLPSolve1()
{
  const auto& m_oracle = Environment::oracle();

  int n = m_instance.size();

  lprec *lp;
  lp = make_lp(0, n * n); // (#rows, #columns = #variables)
  /**
   * SEVERE:   Only severe messages are reported. Errors.
   * CRITICAL: Only critical messages are reported. Hard errors like
   *           instability, out of memory, ...
   * Change to critical before submitting.
   */
  set_verbose(lp, SEVERE);

  /** Configure objective function */
  set_minim(lp);
  double *c = (double *)malloc((n * n + 1) * sizeof(double));
  c[0] = 0; // Element 0 is ignored by LP Solve
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      c[i * n + j + 1] = m_oracle.getCrossings(m_instance[i], m_instance[j]);
    }
  }
  set_obj_fn(lp, c);

  /** Transitivity constraints */
  memset(c, 0, (n * n + 1) * sizeof(double));
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      for (int k = 0; k < n; k++)
      {
        if (i == j or i == k or j == k)
          continue;
        c[i * n + j + 1] = c[j * n + k + 1] = 1;
        c[i * n + k + 1] = -1;
        add_constraint(lp, c, LE, 1);
        c[i * n + j + 1] = c[j * n + k + 1] = c[i * n + k + 1] = 0;
      }
    }
  }

  /** Exactly one constraints */
  for (int i = 0; i < n; i++)
  {
    for (int j = i + 1; j < n; j++)
    {
      c[i * n + j + 1] = c[j * n + i + 1] = 1;
      add_constraint(lp, c, EQ, 1);
      c[i * n + j + 1] = c[j * n + i + 1] = 0;
    }
  }
  free(c); // No longer need it

  /** 0-1 variables constraint */
  for (int i = 1; i <= n * n; i++)
  {
    set_binary(lp, i, TRUE);
  }

  if (::solve(lp))
  {
    throw std::invalid_argument("Houston, we have a problem :-(\n");
  }

  /**
   * Create vector with how many successors each vertex in B has.
   * Sort this vector and return the vertices in reverse order.
   */
  double *vars = (double *)malloc((n * n) * sizeof(double));
  get_variables(lp, vars);
  std::vector<std::pair<int, int>> sol;
  for (int i = 0; i < n; i++)
  {
    int count_successors = 0;
    for (int j = 0; j < n; j++)
    {
      if (i == j)
      {
        continue;
      }
      count_successors += vars[i * n + j];
    }
    sol.push_back({count_successors, i});
  }
  std::sort(sol.begin(), sol.end());
  for (int i = n - 1; i >= 0; i--)
  {
    m_order.emplace_back(sol[i].second, m_weight[sol[i].second]);
  }

  double z = get_objective(lp);
  delete_lp(lp);
  return (int)z; // Return optimal value
}

/**
 * Integer Program 2
 *
 * The same as the first one, but using half the variables: x_{i,j} s.t. i > j;
 *
 * Variables:
 *   cm:      crossing matrix, cm_{i,j} counts the numbers of crossings that
 *            happen if B_i comes before B_j in the ordering.
 *   x_{i,j}: 0-1 variable that is 1 iff B_i appears before B_j in the ordering
 *
 * Formulation:
 *   minimize \sum_{i > j} cm_{i,j} x_{i,j} - \sum_{i < j} cm_{i,j} x_{j,i}
 *
 *   subject to: x_{i,j} + x_{j,k} - x_{i,k} \leq 1 \forall i \neq j \neq k
 *               (transitivity constraint, but caution with the indexes)
 *
 *               x_{i,j} \in \{0, 1\} \forall i, j
 *               (integer program with 0-1 variables)
 */

std::pair<int, bool> IntegerProgrammingSolver::index2(int i, int j)
{
  assert(i != j);
  int index = (i > j) ? i * (i - 1) / 2 + j : j * (j - 1) / 2 + i;
  if (i > j)
    return {index + 1, 0};
  else
    return {index + 1, 1};
}

int IntegerProgrammingSolver::solveWithLPSolve2()
{
  const auto& m_oracle = Environment::oracle();

  int n = m_instance.size();
  int index, b;
  int columns = n * (n - 1) / 2;

  lprec *lp;
  lp = make_lp(0, columns); // (#rows, #columns = #variables)
  /**
   * SEVERE:   Only severe messages are reported. Errors.
   * CRITICAL: Only critical messages are reported. Hard errors like
   *           instability, out of memory, ...
   * Change to critical before submitting.
   */
  set_verbose(lp, SEVERE);

  /** Configure objective function */
  set_minim(lp);
  double *c = (double *)malloc((columns + 1) * sizeof(double));
  c[0] = 0; // Element 0 is ignored by LP Solve
  int objective_offset = 0;
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < i; j++)
    {
      int cm_ij = m_oracle.getCrossings(m_instance[i], m_instance[j]);
      int cm_ji = m_oracle.getCrossings(m_instance[j], m_instance[i]);
      c[index2(i, j).first] = cm_ij - cm_ji;
      objective_offset += cm_ji;
    }
  }
  set_obj_fn(lp, c);

  /** Transitivity constraints */
  memset(c, 0, (columns + 1) * sizeof(double));
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      for (int k = 0; k < n; k++)
      {
        if (i == j or i == k or j == k)
          continue;
        int rhs = 1;

        std::tie(index, b) = index2(i, j);
        if (!b)
        {
          c[index] = 1;
        }
        else
        {
          c[index] = -1;
          rhs -= 1;
        }

        std::tie(index, b) = index2(j, k);
        if (!b)
        {
          c[index] = 1;
        }
        else
        {
          c[index] = -1;
          rhs -= 1;
        }

        std::tie(index, b) = index2(i, k);
        if (!b)
        {
          c[index] = -1;
        }
        else
        {
          c[index] = 1;
          rhs += 1;
        }

        add_constraint(lp, c, LE, rhs);
        c[index2(i, j).first] = c[index2(j, k).first] = 0;
        c[index2(i, k).first] = 0;
      }
    }
  }
  free(c); // No longer need it

  /** 0-1 variables constraint */
  for (int i = 1; i <= columns; i++)
  {
    set_binary(lp, i, TRUE);
  }

  if (::solve(lp))
  {
    throw std::invalid_argument("Houston, we have a problem :-(\n");
  }

  /**
   * Create vector with how many successors each vertex in B has.
   * Sort this vector and return the vertices in reverse order.
   */
  double *vars = (double *)malloc((n * n) * sizeof(double));
  get_variables(lp, vars);
  std::vector<std::pair<int, int>> sol;
  for (int i = 0; i < n; i++)
  {
    int count_successors = 0;
    for (int j = 0; j < n; j++)
    {
      if (i == j)
      {
        continue;
      }
      std::tie(index, b) = index2(i, j);
      index--;
      count_successors += !b ? vars[index] : 1 - vars[index];
    }
    sol.push_back({count_successors, i});
  }
  std::sort(sol.begin(), sol.end());
  for (int i = n - 1; i >= 0; i--)
  {
    m_order.emplace_back(sol[i].second, m_weight[sol[i].second]);
  }

  double z = get_objective(lp);
  delete_lp(lp);
  return (int)z + objective_offset; // Return optimal value
}

} // namespace ip
} // namespace solver
} // namespace banana
