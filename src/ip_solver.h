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

#ifndef __PACE2024__IP_SOLVER_HPP
#define __PACE2024__IP_SOLVER_HPP

#include "environment.h"
#include "bipartite_graph.h"
#include "meta_solver.h"

#include <stdexcept>

namespace banana {
namespace solver {
namespace ip {

class IntegerProgrammingSolverBase
  : public MetaSolver<graph::BipartiteGraph, int>
{
public:
  IntegerProgrammingSolverBase(graph::BipartiteGraph G)
    : MetaSolver<graph::BipartiteGraph, int>(G) {};
  ~IntegerProgrammingSolverBase() = default;
  virtual int solve() = 0;
};

/**
 * This class defines an integer programming solver for the OSCM problem. It is
 * an abstract class which will be extended by each solver.
 */
template<class T, class U>
class IntegerProgrammingSolver : public IntegerProgrammingSolverBase
{
public:
  IntegerProgrammingSolver(graph::BipartiteGraph G);
  ~IntegerProgrammingSolver() = default;
  int solve() override;

 protected:
  /**
   * Simple formulation
   *
   * Variables:
   *   x_{i,j}: 0-1 variable that is 1 iff B_i appears before B_j in the
   *            ordering
   *
   * Formulation:
   *   minimize \sum_{i,j} cm_{i,j} \cdot x_{i,j}
   *
   *   subject to: x_{i,j} + x_{j,k} - x_{i,k} \leq 1 \forall i \neq j \neq k
   *               (transitivity constraint)
   *
   *               x_{i,j} + x_{j, i} = 1 \forall i < j
   *               (exactly one is true, we actually only need at least one
   *                true)
   *
   *               x_{i,j} \in \{0, 1\} \forall i, j
   *               (integer program with 0-1 variables)
   */
  virtual int simple() = 0;

  /**
   * Shorter (simple) formulation
   *
   * The same as the first one, but using half the variables: x_{i,j}
   * s.t. i > j;
   *
   * Variables:
   *   x_{i,j} | i > j: 0-1 variable that is 1 iff B_i appears before B_j in the
   *   ordering
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
  virtual int shorter() = 0;

  /**
   * Quadratic formulation
   *
   * TODO: explain @mvkaio?
   */
  virtual int quadratic() = 0;

  /**
   * Vinicius' formulation
   *
   * TODO: explain @mvkaio?
   */
  virtual int vini() = 0;

  /** TODO: explain @mvkaio? */
  void computeDeltas();
  /** Prefix Constraints On X 
   * 
   *  By evaluating the number of crossings involving a vertex 'p' considering
   *  if it is used in the beginning of the order (L), or the end (R), we can
   *  infer that 'p' isn't too far from the end in which this crossing number
   *  is minimal.
   *
   *  More formally, if there are 'i' vertices before 'p' in the order, there
   *  is at least S = \sum_{i smallest values of} C_{jp} - C_{pj} crossings.
   *  Therefore, if L + S > L, it is never optimal to add 'i' or more vertices
   *  before 'p'.
   *
   *  These constraints can be added in any formulation that uses the variables
   *  'X' indexed by the function triangularIndex.
   */
  virtual void xPrefix(T* program, U& vars) = 0;
  /** TODO: implement and explain? @mvkaio */
  virtual void yPrefix(T* program, U& vars) = 0;
  /** TODO: explain */
  std::pair<int, bool> triangularIndex(int i, int j);
  /** TODO: explain */
  int yIndex(int i, int j, int n, int offset);
};

template<class T, class U>
IntegerProgrammingSolver<T,U>::
IntegerProgrammingSolver(graph::BipartiteGraph graph)
    : IntegerProgrammingSolverBase(graph)
{}

template<class T, class U>
int IntegerProgrammingSolver<T,U>::solve()
{
  options::HolderIP ip_options = Environment::options().ip;
  switch (ip_options.formulation)
  {
    case options::IPFormulation::SIMPLE:
      return simple();
      break;
    case options::IPFormulation::SHORTER:
      return shorter();
      break;
    case options::IPFormulation::QUADRATIC:
      return quadratic();
      break;
    case options::IPFormulation::VINI:
      return vini();
      break;
    default:
      break;
  }
  throw std::runtime_error("Do the L");
}

template<class T, class U>
std::pair<int, bool> IntegerProgrammingSolver<T,U>::
triangularIndex(int i, int j)
{
  assert(i != j);
  int index = (i > j) ? i * (i - 1) / 2 + j : j * (j - 1) / 2 + i;
  if (i > j) { return {index, 0}; }
  else { return {index, 1}; }
}

template<class T, class U>
int IntegerProgrammingSolver<T,U>::yIndex(int i, int j, int n, int offset)
{
  int index = i * n + j;
  index += offset;
  return index;
}

} // namespace ip
} // namespace solver
} // namespace banana

#endif // __PACE2024__IP_SOLVER_HPP
