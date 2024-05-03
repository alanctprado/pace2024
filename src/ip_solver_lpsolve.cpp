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
 * LPSolve extension of the integer programming solver
 */

#include "ip_solver_lpsolve.h"
#include "../lp_solve_5.5/lp_lib.h"
#include "ip_solver.h"

#include <numeric>
#include <stdexcept>

namespace banana {
namespace solver {
namespace ip {

LPSolveSolver::LPSolveSolver(graph::BipartiteGraph graph)
    : IntegerProgrammingSolver<lprec, std::vector<double>>(graph)
{}

int LPSolveSolver::simple()
{
  std::vector<std::vector<int>> cm = m_graph.buildCrossingMatrix();

  int n = m_graph.countVerticesB();
  int offset = m_graph.countVerticesA();

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
  std::vector<double> c(n * n + 1);
  c[0] = 0;    // Element 0 is ignored by LP Solve
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      c[i * n + j + 1] = cm[i][j];
    }
  }
  set_obj_fn(lp, c.data());

  /** Transitivity constraints */
  std::fill(c.begin(), c.end(), 0);
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
        add_constraint(lp, c.data(), LE, 1);
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
      add_constraint(lp, c.data(), EQ, 1);
      c[i * n + j + 1] = c[j * n + i + 1] = 0;
    }
  }

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
    m_order.push_back(sol[i].second + offset);
  }

  double z = get_objective(lp);
  delete_lp(lp);
  return round(z);    // Return optimal value
}

int LPSolveSolver::shorter()
{
  std::vector<std::vector<int>> cm = m_graph.buildCrossingMatrix();

  int n = m_graph.countVerticesB();
  int index, b;
  int offset = m_graph.countVerticesA();
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
  std::vector<double> c(columns + 1);
  c[0] = 0;    // Element 0 is ignored by LP Solve
  int objective_offset = 0;
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < i; j++)
    {
      c[triangularIndex(i, j).first + 1] = cm[i][j] - cm[j][i];
      objective_offset += cm[j][i];
    }
  }
  set_obj_fn(lp, c.data());

  /** Transitivity constraints */
  std::fill(c.begin(), c.end(), 0);
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      for (int k = 0; k < n; k++)
      {
        if (i == j or i == k or j == k)
          continue;
        int rhs = 1;

        std::tie(index, b) = triangularIndex(i, j);
        index++;
        if (!b) { c[index] = 1; }
        else { c[index] = -1; rhs -= 1; }

        std::tie(index, b) = triangularIndex(j, k);
        index++;
        if (!b) { c[index] = 1; }
        else { c[index] = -1; rhs -= 1; }

        std::tie(index, b) = triangularIndex(i, k);
        index++;
        if (!b) { c[index] = -1; }
        else { c[index] = 1; rhs += 1; }
        
        add_constraint(lp, c.data(), LE, rhs);
        c[triangularIndex(i, j).first + 1] = 0;
        c[triangularIndex(j, k).first + 1] = 0;
        c[triangularIndex(i, k).first + 1] = 0;
      }
    }
  }

  /** Prefix constraints */
  const auto& opt = Environment::options().ip.prefixConstraints;
  if (opt == options::IPPrefixConstraints::X)
  {
    xPrefix(lp, c);
  }
  /** Can't add Y constraints */
  assert(opt != options::IPPrefixConstraints::Y);
  assert(opt != options::IPPrefixConstraints::BOTH);

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
  double* vars = (double*) malloc((columns) * sizeof(double));
  get_variables(lp, vars);
  std::vector<std::pair<int, int>> sol;
  for (int i = 0; i < n; i++)
  {
    int count_successors = 0;
    for (int j = 0; j < n; j++)
    {
      if (i == j) { continue; }
      std::tie(index, b) = triangularIndex(i, j);
      count_successors += !b ? vars[index] : 1 - vars[index];
    }
    sol.push_back({count_successors, i});
  }
  std::sort(sol.begin(), sol.end());
  for (int i = n - 1; i >= 0; i--)
  {
    m_order.push_back(sol[i].second + offset);
  }

  double z = get_objective(lp);
  delete_lp(lp);

  return round(z) + objective_offset;    // Return optimal value
}

int LPSolveSolver::quadratic()
{
  std::vector<std::vector<int>> cm = m_graph.buildCrossingMatrix();

  int n = m_graph.countVerticesB();
  int columns = n * (n - 1) / 2 + n * n;
  int offset = n * (n - 1) / 2;

  lprec* lp;
  lp = make_lp(0, columns);    // (#rows, #columns = #variables)
  /**
    * SEVERE:   Only severe messages are reported. Errors.
    * CRITICAL: Only critical messages are reported. Hard errors like
    *           instability, out of memory, ...
    * Change to critical before submitting.
    */
  set_verbose(lp, SEVERE);

  /** Configure objective function */
  set_minim(lp);
  std::vector<double> c(columns + 1);
  int objective_offset = 0;
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < i; j++)
    {
      c[triangularIndex(i, j).first + 1] = cm[i][j] - cm[j][i];
      objective_offset += cm[j][i];
    }
  }
  set_obj_fn(lp, c.data());

  std::fill(c.begin(), c.end(), 0);
  for (int k = 0; k < n; k++)
  {
    for (int i = 0; i < n; i++) { c[yIndex(i, k, n, offset)] = 1; }
    add_constraint(lp, c.data(), EQ, k + 1);
    for (int i = 0; i < n; i++) { c[yIndex(i, k, n, offset)] = 0; }
  }

  for (int k = 0; k < n - 1; k++)
  {
    for (int i = 0; i < n; i++) 
    {
      c[yIndex(i, k, n, offset)] = 1;
      c[yIndex(i, k + 1, n, offset)] = -1;
      add_constraint(lp, c.data(), LE, 0);
      c[yIndex(i, k, n, offset)] = 0;
      c[yIndex(i, k + 1, n, offset)] = 0;
    }
  }

  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++) 
    {
      if (i == j) continue;
      int rhs = 0;
      for (int k = 0; k < n; k++)
      {
        c[yIndex(i, k, n, offset)] = 1;
        c[yIndex(j, k, n, offset)] = -1;
        auto [index, b] = triangularIndex(j, i);
        index++;
        if (!b) { c[index] = -(n - 1); }
        else { c[index] = n - 1; rhs += n - 1; }
      }
      add_constraint(lp, c.data(), LE, rhs);
      for (int k = 0; k < n; k++)
      {
        c[yIndex(i, k, n, offset)] = 0;
        c[yIndex(j, k, n, offset)] = 0;
        c[triangularIndex(j, i).first + 1] = 0;
      }
    }
  }

  /** Prefix constraints */
  const auto& opt = Environment::options().ip.prefixConstraints;
  if (opt == options::IPPrefixConstraints::X ||
      opt == options::IPPrefixConstraints::BOTH)
  {
    xPrefix(lp, c);
  }

  if (opt == options::IPPrefixConstraints::Y ||
      opt == options::IPPrefixConstraints::BOTH)
  {
    yPrefix(lp, c);
  }

  /** 0-1 variables constraint */
  for (int i = 1; i <= columns; i++) { set_binary(lp, i, TRUE); }

  if (::solve(lp))
  {
    throw std::runtime_error("Houston, we have a problem :-(\n");
  }

  /**
    * Create vector with how many successors each vertex in B has.
    * Sort this vector and return the vertices in reverse order.
    */
  double* vars = (double*) malloc((columns) * sizeof(double));
  get_variables(lp, vars);

  std::vector<std::pair<int, int>> sol;
  for (int i = 0; i < n; i++)
  {
    int count_successors = 0;
    for (int j = 0; j < n; j++)
    {
      if (i == j) { continue; }
      auto [index, b] = triangularIndex(i, j);
      count_successors += !b ? vars[index] : 1 - vars[index];
    }
    sol.push_back({count_successors, i});
  }

  offset = m_graph.countVerticesA();
  std::sort(sol.begin(), sol.end());
  for (int i = n - 1; i >= 0; i--)
  {
    m_order.push_back(sol[i].second + offset);
  }

  double z = get_objective(lp);
  delete_lp(lp);
  return round(z) + objective_offset;    // Return optimal value
}

void LPSolveSolver::xPrefix(lprec* lp, std::vector<double>& c)
{
  std::vector<std::vector<int>> cm = m_graph.buildCrossingMatrix();
  int n = m_graph.countVerticesB();
  int offset = m_graph.countVerticesA();

  for (int p = 0; p < n; p++)
  {
    std::vector<int> deltas(n);
    for (int j = 0; j < n; j++)
    {
      /** change in crossings when moving j to the left of p */
      /* TODO: Why does this work? */
      /* deltas[j] = cm[j][p] - cm[p][j]; */
      deltas[j] = cm[p][j] - cm[j][p];
    }

    std::vector<int> order(n - 1);
    std::iota(order.begin(), order.begin() + p, 0);
    std::iota(order.begin() + p, order.end(), p + 1);
    std::sort(order.begin(), order.end(), [&] (int i, int j)
        {
          return deltas[i] < deltas[j];
        });

    /** 
      * The position of 'p' is at most max_prefix, being the first moment 
      * when it's certainly better to put 'p' at the beginning,
      * this is given by the first prefix such that \sum_j delta_j > 0
      */
    int cum_sum = 0, max_prefix = 0;
    for (int j = 0; j < n - 1; j++) {
      cum_sum += deltas[order[j]];
      if (cum_sum > 0) { break; }
      max_prefix++;
    }

    /** 
      * Populates the constraint vector 'c' with the sum of 'x_{jp}' for 
      * every other vertex 'j', when 'p' is fixed.
      *
      * It is used to restrict the position of 'p' to a prefix or a suffix.
      */
    auto _create_constraint = [&] ()
    {
      for (int j = 0; j < p; j++)
      {
        c[triangularIndex(j, p).first + 1] = 1;
      }
      for (int j = p + 1; j < n; j++)
      {
        c[triangularIndex(j, p).first + 1] = -1;
      }
    };

    /** 
      * Resets the constraint vector 'c' to be used by the next constraint.
      */
    auto _undo_constraint = [&] ()
    {
      for (int j = 0; j < p; j++)
      {
        c[triangularIndex(j, p).first + 1] = 0;
      }
      for (int j = p + 1; j < n; j++)
      {
        c[triangularIndex(j, p).first + 1] = 0;
      }
    };

    /** Add constraint pos(p) <= max_prefix */
    if (max_prefix < n - 1)
    {
      _create_constraint();
      int inverted_vars = (n - 1) - p;
      add_constraint(lp, c.data(), LE, max_prefix - inverted_vars);
      _undo_constraint();
    }

    /** 
      * The position of 'p' is at least min_suffix, being the first moment 
      * when it's certainly better to put 'p' at the end,
      * this is given by the first suffix such that \sum_j delta_j < 0
      */
    cum_sum = 0;
    int min_suffix = n - 1;
    for (int j = n - 2; j >= 0; --j)
    {
      cum_sum += deltas[order[j]];
      if (cum_sum < 0) { break; }
      min_suffix--;
    }

    /** Add constraint pos(p) >= min_suffix */
    if (min_suffix > 0)
    {
      _create_constraint();
      int inverted_vars = (n - 1) - p;
      add_constraint(lp, c.data(), GE, min_suffix - inverted_vars);
      _undo_constraint();
    }
  }
}

void LPSolveSolver::yPrefix(lprec* lp, std::vector<double>& c)
{
  throw std::runtime_error("Not implemented.\n");
}

int LPSolveSolver::vini()
{
  std::vector<std::vector<int>> cm = m_graph.buildCrossingMatrix();

  int n = m_graph.countVerticesB();
  int columns = n * (n - 1) / 2 + n * n;
  int offset = n * (n - 1) / 2;

  lprec* lp;
  lp = make_lp(0, columns);    // (#rows, #columns = #variables)
  /**
    * SEVERE:   Only severe messages are reported. Errors.
    * CRITICAL: Only critical messages are reported. Hard errors like
    *           instability, out of memory, ...
    * Change to critical before submitting.
    */
  set_verbose(lp, SEVERE);

  /** Configure objective function */
  set_minim(lp);
  std::vector<double> c(columns + 1);
  int objective_offset = 0;
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < i; j++)
    {
      c[triangularIndex(i, j).first + 1] = cm[i][j] - cm[j][i];
      objective_offset += cm[j][i];
    }
  }
  set_obj_fn(lp, c.data());

  std::fill(c.begin(), c.end(), 0);
  for (int k = 0; k < n; k++)
  {
    for (int i = 0; i < n; i++) { c[yIndex(i, k, n, offset)] = 1; }
    add_constraint(lp, c.data(), EQ, k + 1);
    for (int i = 0; i < n; i++) { c[yIndex(i, k, n, offset)] = 0; }
  }

  for (int k = 0; k < n - 1; k++)
  {
    for (int i = 0; i < n; i++) 
    {
      c[yIndex(i, k, n, offset)] = 1;
      c[yIndex(i, k + 1, n, offset)] = -1;
      add_constraint(lp, c.data(), LE, 0);
      c[yIndex(i, k, n, offset)] = 0;
      c[yIndex(i, k + 1, n, offset)] = 0;
    }
  }

  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++) 
    {
      if (i == j) continue;
      for (int k = 0; k < n; k++)
      {
        int rhs = 0;
        c[yIndex(i, k, n, offset)] = 1;
        c[yIndex(j, k, n, offset)] = -1;
        auto [index, b] = triangularIndex(j, i);
        index++;
        if (!b) { c[index] = -1; }
        else { c[index] = 1; rhs += 1; }
        add_constraint(lp, c.data(), LE, rhs);
        c[yIndex(i, k, n, offset)] = 0;
        c[yIndex(j, k, n, offset)] = 0;
        c[index] = 0;
      }
    }
  }

  /** Prefix constraints */
  const auto& opt = Environment::options().ip.prefixConstraints;
  if (opt == options::IPPrefixConstraints::X ||
      opt == options::IPPrefixConstraints::BOTH)
  {
    xPrefix(lp, c);
  }

  if (opt == options::IPPrefixConstraints::Y ||
      opt == options::IPPrefixConstraints::BOTH)
  {
    yPrefix(lp, c);
  }

  /** 0-1 variables constraint */
  for (int i = 1; i <= columns; i++) { set_binary(lp, i, TRUE); }

  if (::solve(lp))
  {
    throw std::invalid_argument("Houston, we have a problem :-(\n");
  }

  /**
    * Create vector with how many successors each vertex in B has.
    * Sort this vector and return the vertices in reverse order.
    */
  double* vars = (double*) malloc((columns) * sizeof(double));
  get_variables(lp, vars);

  std::vector<std::pair<int, int>> sol;
  for (int i = 0; i < n; i++)
  {
    int count_successors = 0;
    for (int j = 0; j < n; j++)
    {
      if (i == j) { continue; }
      auto [index, b] = triangularIndex(i, j);
      count_successors += !b ? vars[index] : 1 - vars[index];
    }
    sol.push_back({count_successors, i});
  }

  offset = m_graph.countVerticesA();
  std::sort(sol.begin(), sol.end());
  for (int i = n - 1; i >= 0; i--)
  {
    m_order.push_back(sol[i].second + offset);
  }

  double z = get_objective(lp);
  delete_lp(lp);
  return round(z) + objective_offset;    // Return optimal value
}

} // namespace ip
} // namespace solver
} // namespace banana
