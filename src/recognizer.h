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
 * Abstract class for graph recognizers
 */

#ifndef __PACE2024__RECOGNIZER_HPP
#define __PACE2024__RECOGNIZER_HPP

#include "abstract_graph.h"

#include <memory>

/**
 * TO-DO
 */
template <class T>
class Recognizer
{
 public:
  Recognizer(TGraph<T>& graph) : m_graph(graph) {}
  virtual ~Recognizer() {}
  virtual bool check();

 protected:
  const TGraph<T> m_graph;
};

#endif  // __PACE2024__RECOGNIZER_HPP
