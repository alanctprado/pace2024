/******************************************************************************
 * Top contributors (to current version):
 *   Gabriel Lucas, Gabriel Ubiratan, Kaio Vieira
 *
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 */

#ifndef __PACE2024__PREPROCESSING_HPP
#define __PACE2024__PREPROCESSING_HPP

#include "oracle.h"
#include "environment.h"
#include "base_solver.h"

#include <algorithm>
#include <cassert>
#include <vector>
#include <functional>


namespace banana {
namespace solver {

/**
 * Abstract preprocessing routine
 */
class Preprocessing
{
public:
  Preprocessing(Oracle::SubProblem G) : m_instance(G) {}
  virtual ~Preprocessing() {}

  bool lmr_reduction(Oracle::SubProblem &instance);
  bool twins(Oracle::SubProblem &instance);
  bool cut_by_pieces(Oracle::SubProblem &instance);
  bool generalized_twins(Oracle::SubProblem &instance);

  virtual int solve() = 0;
  void explain(std::vector<Oracle::Vertex> &order);

protected:
  Oracle::SubProblem const m_instance;
  std::vector<Oracle::Vertex> m_order;
};

} // namespace solver
} // namespace banana

#endif // __PACE2024__PREPROCESSING_HPP