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
#include "approximation_routine.h"
#include "barycenter_heuristic.h"
#include "crossing_matrix.h"
#include "median_heuristic.h"

#include <numeric>
#include <stdexcept>
#include <iostream>

namespace banana {
namespace solver {
namespace ip {

int search_pair(std::vector<std::pair<int, int>> &orientable_pairs,
                std::pair<int, int> key)
{
  auto it =
      std::lower_bound(orientable_pairs.begin(), orientable_pairs.end(), key);

  if (it == orientable_pairs.end() or *it != key)
    return -1;

  return std::distance(orientable_pairs.begin(), it);
}

enum class PAIR_STATE
{
  FREE,
  OR,
  PRE,
  POS,
};

PAIR_STATE pair_state(std::unordered_map<int, int> &l,
                      std::unordered_map<int, int> &r, std::pair<int, int> key)
{
  auto [i, j] = key;

  if (l[i] == l[j] and r[i] == r[j] and l[i] == r[i])
  {
    return PAIR_STATE::FREE;
  }
  else if (r[i] <= l[j])
  {
    return PAIR_STATE::PRE; // forced ij
  }
  else if (r[j] <= l[i])
  {
    return PAIR_STATE::POS; // forced ji
  }
  else
  {
    return PAIR_STATE::OR;
  }
}

LPSolveSolver::LPSolveSolver(graph::BipartiteGraph graph)
    : IntegerProgrammingSolver<lprec, std::vector<double>>(graph)
{}

int LPSolveSolver::simple()
{
  banana::crossing::CrossingMatrix cm(m_graph);
  std::vector<std::pair<int, int>> orientable_pairs = cm.getOrientablePairs();

  auto intervals = crossing::CrossingMatrix::getIntervals(m_graph);
  std::unordered_map<int, int> l, r;
  l = intervals[0], r = intervals[1];

  const int number_vars = orientable_pairs.size();

  int n = m_graph.countVerticesB();

  lprec *lp;
  lp = make_lp(0, number_vars); // (#rows, #columns = #variables)
  /**
   * SEVERE:   Only severe messages are reported. Errors.
   * CRITICAL: Only critical messages are reported. Hard errors like
   *           instability, out of memory, ...
   * Change to critical before submitting.
   */
  set_verbose(lp, SEVERE);

  /** Configure objective function */
  set_minim(lp);
  std::vector<double> c(number_vars + 1);
  c[0] = 0; // Element 0 is ignored by LP Solve
  for (int k = 1; k <= number_vars; k++)
  {
    auto [i, j] = orientable_pairs[k - 1];
    {
      c[k] = cm(i, j);
    }
  }
  set_obj_fn(lp, c.data());

  /** Heuristic constraints */
  // TODO: Create flag that controls whether this is active

  std::vector<std::unique_ptr<heuristic::ApproximationRoutine>> heuristics;

  // check barycenter heuristic
  heuristics.push_back(std::make_unique<heuristic::barycenter::BarycenterHeuristic>(m_graph));
  heuristics.push_back(std::make_unique<heuristic::median::MedianHeuristic>(m_graph));

  int best_heuristic_objective = -1;

  for (const auto& h : heuristics)
  {
    int obj = h->solve();
    if (best_heuristic_objective == -1 ||
        best_heuristic_objective > obj)
    {
      best_heuristic_objective = obj;
    }
  }

  // we add a constraint saying that the objective value (reusing the
  // values of from the objective loop) is less than or equal to the
  // best objective value from the heuristics
  add_constraint(lp, c.data(), LE, best_heuristic_objective);

  /** Transitivity constraints */
  // NOTE: It can be shown that the first variable can always be assumed to be
  // orientable
  std::fill(c.begin(), c.end(), 0);
  for (int idx = 0; idx < number_vars; idx++)
  {
    auto [i, j] = orientable_pairs[idx];
    for (int k : m_graph.getB())
    {

      if (i == j or i == k or j == k)
        continue;

      PAIR_STATE st_ij = pair_state(l, r, {i, j});
      PAIR_STATE st_jk = pair_state(l, r, {j, k});
      PAIR_STATE st_ik = pair_state(l, r, {i, k});

      assert(st_ij == PAIR_STATE::OR);

      // search indexes
      int idx_ij = idx + 1;
      int idx_jk = search_pair(orientable_pairs, {j, k});
      int idx_ik = search_pair(orientable_pairs, {i, k});

      // the lp uses 1-based indices, the OP vector uses 0-based
      idx_ik++, idx_jk++;

      // check if jk or ik are orientable
      if (st_jk != PAIR_STATE::OR and st_ik != PAIR_STATE::OR)
      {
        continue;
      }

      if (st_jk != PAIR_STATE::OR)
      {
        int forced_jk = -1;
        PAIR_STATE st = pair_state(l, r, {j, k});
        switch (st)
        {
        case PAIR_STATE::OR:
          assert(false);
        case PAIR_STATE::FREE:
          forced_jk = j < k ? 1 : 0;
          break;
        case PAIR_STATE::PRE:
          forced_jk = 1;
          break;
        case PAIR_STATE::POS:
          forced_jk = 0;
          break;
        }

        if (forced_jk == 1)
        {
          // set constraint x_ij + 1 - x_ik <= 1 => x_ij - x_ik <= 0
          c[idx_ij] = 1;
          c[idx_ik] = -1;
          add_constraint(lp, c.data(), LE, 0);
          c[idx_ij] = c[idx_jk] = c[idx_ik] = 0;
        }
      }
      else if (st_ik != PAIR_STATE::OR)
      {
        int forced_ik = -1;
        PAIR_STATE st = pair_state(l, r, {i, k});
        switch (st)
        {
        case PAIR_STATE::OR:
          assert(false);
        case PAIR_STATE::FREE:
          forced_ik = i < k ? 1 : 0;
          break;
        case PAIR_STATE::PRE:
          forced_ik = 1;
          break;
        case PAIR_STATE::POS:
          forced_ik = 0;
          break;
        }

        if (forced_ik == 0)
        {
          // set constraint x_ij + x_jk - 0 <= 1 => x_ij + x_jk <= 1
          c[idx_ij] = c[idx_jk] = 1;
          add_constraint(lp, c.data(), LE, 1);
          c[idx_ij] = c[idx_jk] = c[idx_ik] = 0;
        }
      }
      else
      {
        assert(st_ij == PAIR_STATE::OR and st_jk == PAIR_STATE::OR and
               st_ik == PAIR_STATE::OR);

        // set constraint x_ij + x_jk - x_ik <= 1
        c[idx_ij] = c[idx_jk] = 1;
        c[idx_ik] = -1;
        add_constraint(lp, c.data(), LE, 1);
        c[idx_ij] = c[idx_jk] = c[idx_ik] = 0;
      }
    }
  }

  /** Exactly one constraints */
  for (int idx = 0; idx < number_vars; idx++)
  {
    auto [i, j] = orientable_pairs[idx];

    if (j < i)
      continue;

    int idx_r = search_pair(orientable_pairs, {j, i});

    // NOTE: if this fails, the OPs are assymetric, which is ok
    // we just need to change the code accordingly
    assert(idx_r != -1);

    idx++, idx_r++;

    c[idx] = c[idx_r] = 1;
    add_constraint(lp, c.data(), EQ, 1);
    c[idx] = c[idx_r] = 0;

    idx--;
  }

  /** 0-1 variables constraint */
  for (int i = 1; i <= number_vars; i++)
  {
    set_binary(lp, i, TRUE);
  }

  if (number_vars > 0 and ::solve(lp))
  {
    throw std::invalid_argument("Houston, we have a problem :-(\n");
  }

  /**
   * Create vector with how many successors each vertex in B has.
   * Sort this vector and return the vertices in reverse order.
   */

  std::vector<double> vars(number_vars);
  if (number_vars > 0)
    get_variables(lp, vars.data());
  std::vector<std::pair<int, int>> sol;
  for (int i : m_graph.getB())
  {
    int count_successors = 0;
    for (int j : m_graph.getB())
    {
      if (i == j)
      {
        continue;
      }

      PAIR_STATE st_ij = pair_state(l, r, {i, j});

      if (st_ij == PAIR_STATE::OR)
      {
        // pair is orientable, check IP solution
        int idx_ij = search_pair(orientable_pairs, {i, j});

        count_successors += vars[idx_ij];
      }
      else
      {
        if (st_ij == PAIR_STATE::FREE)
        {
          //  {i, j} is free, we suppose it is decided on the order of the
          //  vertices
          count_successors += i < j ? 1 : 0;
        }
        else if (st_ij == PAIR_STATE::PRE)
        {
          // {i, j} is forced to ij
          count_successors++;
        }
        else if (st_ij == PAIR_STATE::POS)
        {
          // {i, j} is forced to ji
          continue;
        }
        else
        {
          assert(st_ij == PAIR_STATE::OR);
          throw std::runtime_error(
              "An orientable pair was not decided by the PI!\n");
        }
      }
    }
    sol.push_back({count_successors, i});
  }
  std::sort(sol.begin(), sol.end());
  for (auto it = sol.rbegin(); it != sol.rend(); it++)
  {
    m_order.push_back(it->second);
  }

  double z = number_vars > 0 ? get_objective(lp) : 0;
  delete_lp(lp);
  return round(z); // Return optimal value
}

int LPSolveSolver::shorter()
{
  banana::crossing::CrossingMatrix cm(m_graph);
  std::vector<std::pair<int, int>> orientable_pairs = cm.getOrientablePairs();

  // Extract ordered list of indexes where i < j
  std::vector<std::pair<int, int>> pairs;
  for (auto [i, j] : orientable_pairs)
  {
    if (i < j)
      pairs.push_back({i, j});
  }

  auto intervals = crossing::CrossingMatrix::getIntervals(m_graph);
  std::unordered_map<int, int> l, r;
  l = intervals[0], r = intervals[1];

  const int n = m_graph.countVerticesB();

  const int number_vars = pairs.size();

  lprec *lp;
  lp = make_lp(0, number_vars); // (#rows, #columns = #variables)
  /**
   * SEVERE:   Only severe messages are reported. Errors.
   * CRITICAL: Only critical messages are reported. Hard errors like
   *           instability, out of memory, ...
   * Change to critical before submitting.
   */
  set_verbose(lp, SEVERE);

  /** Configure objective function */
  set_minim(lp);
  std::vector<double> c(number_vars + 1);
  c[0] = 0; // Element 0 is ignored by LP Solve
  int objective_offset = 0;
  for (int idx = 1; idx <= number_vars; idx++)
  {
    auto [i, j] = pairs[idx - 1];
    assert(i < j);
    c[idx] = cm(i, j) - cm(j, i);
    objective_offset += cm(j, i);
  }
  set_obj_fn(lp, c.data());

  /** Heuristic constraints */
  // TODO: Create flag that controls whether this is active

  std::vector<std::unique_ptr<heuristic::ApproximationRoutine>> heuristics;

  // check barycenter heuristic
  heuristics.push_back(std::make_unique<heuristic::barycenter::BarycenterHeuristic>(m_graph));
  heuristics.push_back(std::make_unique<heuristic::median::MedianHeuristic>(m_graph));

  int best_heuristic_objective = -1;

  for (const auto& h : heuristics)
  {
    int obj = h->solve();
    if (best_heuristic_objective == -1 ||
        best_heuristic_objective > obj)
    {
      best_heuristic_objective = obj;
    }
  }

  // we add a constraint saying that the objective value (reusing the
  // values of from the objective loop) is less than or equal to the
  // best objective value from the heuristics
  add_constraint(lp, c.data(), LE, best_heuristic_objective);


  /** Transitivity constraints */
  std::fill(c.begin(), c.end(), 0);
  // NOTE: This iterates over orientable_pairs, not pairs, because it needs to
  // check both {i, j} and {j,i}
  for (auto [i, j] : orientable_pairs)
  {
    PAIR_STATE st_ij = pair_state(l, r, {i, j});
    assert(st_ij == PAIR_STATE::OR);

    for (int k : m_graph.getB())
    {
      if (i == j or i == k or j == k)
        continue;
      int rhs = 1;

      PAIR_STATE st_jk = pair_state(l, r, {j, k});
      PAIR_STATE st_ik = pair_state(l, r, {i, k});

      // check if jk or ik are orientable
      if (st_jk != PAIR_STATE::OR and st_ik != PAIR_STATE::OR)
      {
        continue;
      }

      int idx_ij;
      if (i < j)
        idx_ij = search_pair(pairs, {i, j});
      else
        idx_ij = search_pair(pairs, {j, i});

      int idx_jk;
      if (j < k)
        idx_jk = search_pair(pairs, {j, k});
      else
        idx_jk = search_pair(pairs, {k, j});

      int idx_ik;
      if (i < k)
        idx_ik = search_pair(pairs, {i, k});
      else
        idx_ik = search_pair(pairs, {k, i});

      idx_ij++, idx_jk++, idx_ik++;

      switch (st_jk)
      {
      case PAIR_STATE::FREE:
        if (j < k)
          rhs -= 1;
        break;
      case PAIR_STATE::PRE:
        rhs -= 1;
        break;
      case PAIR_STATE::POS:
        break;
      case PAIR_STATE::OR:
        if (j < k)
          c[idx_jk] = 1;
        else
          c[idx_jk] = -1, rhs -= 1;
        break;
      }

      switch (st_ik)
      {
      case PAIR_STATE::FREE:
        if (i < k)
          rhs += 1;
        break;
      case PAIR_STATE::PRE:
        rhs += 1;
        break;
      case PAIR_STATE::POS:
        break;
      case PAIR_STATE::OR:
        if (i < k)
          c[idx_ik] = -1;
        else
          c[idx_ik] = 1, rhs += 1;
        break;
      }

      if (i < j)
      {
        c[idx_ij] = 1;
      }
      else
      {
        c[idx_ij] = -1;
        rhs -= 1;
      }

      // std::cerr << "Add constraint" << std::endl;
      add_constraint(lp, c.data(), LE, rhs);
      c[idx_ij] = 0;
      c[idx_jk] = 0;
      c[idx_ik] = 0;
    }
  }

  /** Prefix constraints */
  // const auto &opt = Environment::options().ip.prefixConstraints;
  // if (opt == options::IPPrefixConstraints::X)
  // {
  //   xPrefix(lp, c);
  // }
  // /** Can't add Y constraints */
  // assert(opt != options::IPPrefixConstraints::Y);
  // assert(opt != options::IPPrefixConstraints::BOTH);

  /** 0-1 variables constraint */
  for (int i = 1; i <= number_vars; i++)
  {
    set_binary(lp, i, TRUE);
  }

  if (number_vars > 0 and ::solve(lp))
  {
    throw std::runtime_error("Hate you LPSolve! ;-;\n");
  }

  /**
   * Create vector with how many successors each vertex in B has.
   * Sort this vector and return the vertices in reverse order.
   */
  std::vector<double> vars(number_vars);
  if (number_vars > 0)
    get_variables(lp, vars.data());
  std::vector<std::pair<int, int>> sol;
  int pre, pos, ors, frees;
  pre = pos = ors = frees = 0;
  for (int i : m_graph.getB())
  {
    int count_successors = 0;
    for (int j : m_graph.getB())
    {
      if (i == j)
      {
        continue;
      }

      PAIR_STATE st_ij = pair_state(l, r, {i, j});
      if (st_ij == PAIR_STATE::OR)
      {
        ors++;
        // pair is orientable, check IP solution

        int idx_ij = search_pair(pairs, {i, j});
        int idx_ji = search_pair(pairs, {j, i});

        if (i < j)
        {
          assert(idx_ij != -1);
          assert(idx_ji == -1);
          count_successors += vars[idx_ij];
        }
        else
        {
          assert(idx_ji != -1);
          assert(idx_ij == -1);
          count_successors += 1 - vars[idx_ji];
        }
      }
      else
      {
        if (st_ij == PAIR_STATE::FREE)
        {
          frees++;
          //  {i, j} is free, we suppose it is decided on the order of the
          //  vertices
          count_successors += i < j ? 1 : 0;
        }
        else if (st_ij == PAIR_STATE::PRE)
        {
          pre++;
          // {i, j} is forced to ij
          count_successors++;
        }
        else if (st_ij == PAIR_STATE::POS)
        {
          pos++;
          // {i, j} is forced to ji
          continue;
        }
        else
        {
          assert(st_ij == PAIR_STATE::OR);
          throw std::runtime_error(
              "An orientable pair was not decided by the PI!\n");
        }
      }
    }
    sol.push_back({count_successors, i});
  }
  // std::cerr << "N: " << n << " Pre: " << pre << " Pos: " << pos << " Free: "
  // << frees << " Ors: " << ors << std::endl;
  std::sort(sol.begin(), sol.end());
  for (auto it = sol.rbegin(); it != sol.rend(); it++)
  {
    m_order.push_back(it->second);
  }

  double z = number_vars > 0 ? get_objective(lp) : 0;
  delete_lp(lp);
  return round(z) + objective_offset; // Return optimal value
}

int LPSolveSolver::quadratic()
{
  std::vector<std::vector<int>> cm = m_graph.buildCrossingMatrix();

  int n = m_graph.countVerticesB();
  int columns = n * (n - 1) / 2 + n * n;
  int offset = n * (n - 1) / 2;

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
    for (int i = 0; i < n; i++)
    {
      c[yIndex(i, k, n, offset)] = 1;
    }
    add_constraint(lp, c.data(), EQ, k + 1);
    for (int i = 0; i < n; i++)
    {
      c[yIndex(i, k, n, offset)] = 0;
    }
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
      if (i == j)
        continue;
      int rhs = 0;
      for (int k = 0; k < n; k++)
      {
        c[yIndex(i, k, n, offset)] = 1;
        c[yIndex(j, k, n, offset)] = -1;
        auto [index, b] = triangularIndex(j, i);
        index++;
        if (!b)
        {
          c[index] = -(n - 1);
        }
        else
        {
          c[index] = n - 1;
          rhs += n - 1;
        }
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
  const auto &opt = Environment::options().ip.prefixConstraints;
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
  for (int i = 1; i <= columns; i++)
  {
    set_binary(lp, i, TRUE);
  }

  if (::solve(lp))
  {
    throw std::runtime_error("Houston, we have a problem :-(\n");
  }

  /**
   * Create vector with how many successors each vertex in B has.
   * Sort this vector and return the vertices in reverse order.
   */
  double *vars = (double *)malloc((columns) * sizeof(double));
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
  return round(z) + objective_offset; // Return optimal value
}

void LPSolveSolver::xPrefix(lprec *lp, std::vector<double> &c)
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
    std::sort(order.begin(), order.end(),
              [&](int i, int j) { return deltas[i] < deltas[j]; });

    /**
     * The position of 'p' is at most max_prefix, being the first moment
     * when it's certainly better to put 'p' at the beginning,
     * this is given by the first prefix such that \sum_j delta_j > 0
     */
    int cum_sum = 0, max_prefix = 0;
    for (int j = 0; j < n - 1; j++)
    {
      cum_sum += deltas[order[j]];
      if (cum_sum > 0)
      {
        break;
      }
      max_prefix++;
    }

    /**
     * Populates the constraint vector 'c' with the sum of 'x_{jp}' for
     * every other vertex 'j', when 'p' is fixed.
     *
     * It is used to restrict the position of 'p' to a prefix or a suffix.
     */
    auto _create_constraint = [&]() {
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
    auto _undo_constraint = [&]() {
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
      if (cum_sum < 0)
      {
        break;
      }
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

void LPSolveSolver::yPrefix(lprec *lp, std::vector<double> &c)
{
  throw std::runtime_error("Not implemented.\n");
}

int LPSolveSolver::vini()
{
  std::vector<std::vector<int>> cm = m_graph.buildCrossingMatrix();

  int n = m_graph.countVerticesB();
  int columns = n * (n - 1) / 2 + n * n;
  int offset = n * (n - 1) / 2;

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
    for (int i = 0; i < n; i++)
    {
      c[yIndex(i, k, n, offset)] = 1;
    }
    add_constraint(lp, c.data(), EQ, k + 1);
    for (int i = 0; i < n; i++)
    {
      c[yIndex(i, k, n, offset)] = 0;
    }
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
      if (i == j)
        continue;
      for (int k = 0; k < n; k++)
      {
        int rhs = 0;
        c[yIndex(i, k, n, offset)] = 1;
        c[yIndex(j, k, n, offset)] = -1;
        auto [index, b] = triangularIndex(j, i);
        index++;
        if (!b)
        {
          c[index] = -1;
        }
        else
        {
          c[index] = 1;
          rhs += 1;
        }
        add_constraint(lp, c.data(), LE, rhs);
        c[yIndex(i, k, n, offset)] = 0;
        c[yIndex(j, k, n, offset)] = 0;
        c[index] = 0;
      }
    }
  }

  /** Prefix constraints */
  const auto &opt = Environment::options().ip.prefixConstraints;
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
  double *vars = (double *)malloc((columns) * sizeof(double));
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
  return round(z) + objective_offset; // Return optimal value
}

} // namespace ip
} // namespace solver
} // namespace banana
