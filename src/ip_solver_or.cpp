/******************************************************************************
 * Top contributors (to current version):
 *   Luis Higino
 *
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 *
 * Or Tools extension of the integer programming solver
 */

#ifdef USE_OR_TOOLS

#include "ip_solver_or.h"

#include "options.h"

#include <cstdlib>
#include <math.h>
#include <numeric>
#include <ostream>
#include <stdexcept>

#include <iostream>

namespace banana {
namespace solver {
namespace ip {

OrToolsSolver::OrToolsSolver(graph::BipartiteGraph graph,
                             const options::IPSubSolverMode &subSolver)
    : IntegerProgrammingSolver<MPSolver, std::vector<MPVariable>>(graph),
      sub_solver(subSolver)
{}

int OrToolsSolver::simple() { throw std::runtime_error("Not implemented.\n"); }

int OrToolsSolver::shorter()
{
  std::vector<std::vector<int>> cm = m_graph.buildCrossingMatrix();

  unsigned n = m_graph.countVerticesB();
  unsigned columns = n * (n - 1) / 2;

  std::unique_ptr<MPSolver> model;

  switch (sub_solver)
  {
  case options::IPSubSolverMode::NONE:
    throw std::runtime_error("OR-TOOLS needs to use a sub solver!.\n");
  case options::IPSubSolverMode::CBC:
    model = std::unique_ptr<MPSolver>(MPSolver::CreateSolver("CBC"));
    break;

  case options::IPSubSolverMode::HIGHS:
    model = std::unique_ptr<MPSolver>(
        MPSolver::CreateSolver("HIGHS_LINEAR_PROGRAMMING"));
    break;
  case options::IPSubSolverMode::CP_MIP:
    model = std::unique_ptr<MPSolver>(MPSolver::CreateSolver("CPLEX_MIP"));
    break;

  case options::IPSubSolverMode::CP_SAT:
    model = std::unique_ptr<MPSolver>(MPSolver::CreateSolver("CP_SAT"));
    break;

  case options::IPSubSolverMode::GLPK:
    model = std::unique_ptr<MPSolver>(MPSolver::CreateSolver("GLPK_MIP"));
    break;
  }

  if (!model)
  {
    throw std::runtime_error("Sub solver unavailable!");
  }

  std::vector<MPVariable *> variables;
  model->MakeBoolVarArray((int)columns, std::string{"x"}, &variables);

  MPObjective *const objective = model->MutableObjective();

  int objective_offset = 0;
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < i; j++)
    {
      objective->SetCoefficient(variables[triangularIndex(i, j).first],
                                cm[i][j] - cm[j][i]);
      objective_offset += cm[j][i];
    }
  }

  objective->SetMinimization();
  objective->SetOffset(objective_offset);

  /** Transitivity constraints */
  int index, b;
  double *coeffs = (double *)malloc(3 * sizeof(double));
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      for (int k = 0; k < n; k++)
      {

        MPConstraint *constraint = model->MakeRowConstraint();
        if (i == j or i == k or j == k)
          continue;
        double rhs = 1;

        std::tie(index, b) = triangularIndex(i, j);
        if (!b)
        {
          constraint->SetCoefficient(variables[index], 1);
        }
        else
        {
          constraint->SetCoefficient(variables[index], -1);
          rhs -= 1;
        }

        std::tie(index, b) = triangularIndex(j, k);
        if (!b)
        {
          constraint->SetCoefficient(variables[index], 1);
        }
        else
        {
          constraint->SetCoefficient(variables[index], -1);
          rhs -= 1;
        }

        std::tie(index, b) = triangularIndex(i, k);
        if (!b)
        {
          constraint->SetCoefficient(variables[index], -1);
        }
        else
        {
          constraint->SetCoefficient(variables[index], 1);
          rhs += 1;
        }

        // constraint->SetLB(0);
        constraint->SetUB(rhs);
      }
    }
  }

  /** Prefix constraints */
  // const auto& opt = Environment::options().ip.prefixConstraints;
  // if (opt == options::IPPrefixConstraints::X ||
  //     opt == options::IPPrefixConstraints::BOTH)
  // {
  //   xPrefix(model.get(), variables);
  // }

  // if (opt == options::IPPrefixConstraints::Y ||
  //     opt == options::IPPrefixConstraints::BOTH)
  // {
  //   yPrefix(model.get(), variables);
  // }

  const MPSolver::ResultStatus result_status = model->Solve();

  if (result_status != MPSolver::OPTIMAL)
  {
    throw std::runtime_error("Houston, we have a problem! :q\n");
  }

  /**
   * Create vector with how many successors each vertex in B has.
   * Sort this vector and return the vertices in reverse order.
   */
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
      std::tie(index, b) = triangularIndex(i, j);
      double value = variables[index]->solution_value();
      count_successors += !b ? value : 1 - value;
    }
    sol.push_back({count_successors, i});
  }

  int offset = m_graph.countVerticesA();
  std::sort(sol.begin(), sol.end());
  for (int i = n - 1; i >= 0; i--)
  {
    m_order.push_back(sol[i].second + offset);
  }

  double z = objective->Value();

  return round(z); // Return optimal value
}

int OrToolsSolver::quadratic()
{
  throw std::runtime_error("Not implemented.\n");
}

int OrToolsSolver::vini() { throw std::runtime_error("Not implemented.\n"); }

void OrToolsSolver::xPrefix(MPSolver *model, std::vector<MPVariable> &vars)
{
  throw std::runtime_error("Not implemented.\n");
}

void OrToolsSolver::yPrefix(MPSolver *model, std::vector<MPVariable> &vars)
{
  throw std::runtime_error("Not implemented.\n");
}

} // namespace ip
} // namespace solver
} // namespace banana

#endif
