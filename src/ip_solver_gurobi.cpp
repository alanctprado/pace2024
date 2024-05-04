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

#include "ip_solver_gurobi.h"
#include "../gurobi/include/gurobi_c++.h"

#include <cstdlib>
#include <math.h>
#include <numeric>
#include <ostream>
#include <stdexcept>

#include <iostream>


namespace banana {
namespace solver {
namespace ip {

GurobiSolver::GurobiSolver(graph::BipartiteGraph graph)
    : IntegerProgrammingSolver<GRBModel, GRBVar>(graph)
{}

int GurobiSolver::simple()
{
  throw std::runtime_error("Not implemented.\n");
}

int GurobiSolver::shorter()
{
  std::vector<std::vector<int>> cm = m_graph.buildCrossingMatrix();

  unsigned n = m_graph.countVerticesB();
  unsigned columns = n * (n - 1) / 2;

  GRBEnv env = GRBEnv(true);
  env.set(GRB_IntParam_OutputFlag, 0);
  // env.set("LogFile", "gurobi-solver.log");
  env.start();

  GRBModel model = GRBModel(env);
  model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);

  double* ub = (double*)malloc(columns * sizeof(double));  // Is this necessary, though?
  for (unsigned i = 0; i < columns; i++)
  {
    ub[i] = 1;
  }

  double* obj = (double*)malloc(columns * sizeof(double));
  int objective_offset = 0;
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < i; j++)
    {
      obj[triangularIndex(i, j).first] = cm[i][j] - cm[j][i];
      objective_offset += cm[j][i];
    }
  }

  char* type = (char*)malloc(columns * sizeof(char));
  for (unsigned i = 0; i < columns; i++)
  {
    type[i] = GRB_BINARY;
  }

  GRBVar* variables = model.addVars(NULL, ub, obj, type, NULL, columns);

  free(ub); free(obj); free(type);

  /** Transitivity constraints */
  int index, b;
  double* coeffs = (double*)malloc(3 * sizeof(double));
  GRBVar* vars = (GRBVar*)malloc(3 * sizeof(GRBVar));
  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      for (int k = 0; k < n; k++)
      {
        if (i == j or i == k or j == k) continue;
        double rhs = 1;

        std::tie(index, b) = triangularIndex(i, j);
        vars[0] = variables[index];
        if (!b) { coeffs[0] = 1; }
        else { coeffs[0] = -1; rhs -= 1; }

        std::tie(index, b) = triangularIndex(j, k);
        vars[1] = variables[index];
        if (!b) { coeffs[1] = 1; }
        else { coeffs[1] = -1; rhs -= 1; }

        std::tie(index, b) = triangularIndex(i, k);
        vars[2] = variables[index];
        if (!b) { coeffs[2] = -1; }
        else { coeffs[2] = 1; rhs += 1; }

        GRBLinExpr constraint;
        constraint.addTerms(coeffs, vars, 3);
        model.addConstr(constraint, GRB_LESS_EQUAL, rhs);
      }
    }
  }

  /** Prefix constraints */
  const auto& opt = Environment::options().ip.prefixConstraints;
  if (opt == options::IPPrefixConstraints::X ||
      opt == options::IPPrefixConstraints::BOTH)
  {
    xPrefix(&model, *variables);
  }

  if (opt == options::IPPrefixConstraints::Y ||
      opt == options::IPPrefixConstraints::BOTH)
  {
    yPrefix(&model, *variables);
  }

  model.optimize();

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
      if (i == j) { continue; }
      std::tie(index, b) = triangularIndex(i, j);
      double value = variables[index].get(GRB_DoubleAttr_X);
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

  double z = model.get(GRB_DoubleAttr_ObjVal);

  return round(z) + objective_offset;    // Return optimal value
}

int GurobiSolver::quadratic()
{
  throw std::runtime_error("Not implemented.\n");
}

void GurobiSolver::xPrefix(GRBModel* model, GRBVar& vars)
{
  throw std::runtime_error("Not implemented.\n");
}

void GurobiSolver::yPrefix(GRBModel* model, GRBVar& vars)
{
  throw std::runtime_error("Not implemented.\n");
}

int GurobiSolver::vini()
{
  throw std::runtime_error("Not implemented.\n");
}

} // namespace ip
} // namespace solver
} // namespace banana
