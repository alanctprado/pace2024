/******************************************************************************
 * Top contributors (to current version):
 *   Laila Melo
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 *
 * Outerplanar Recognizer for biconected components
 * It assumes that every edge uv in m_graph.edges, u<v
 */

#ifndef __PACE2024__OUTERPLANAR_RECOGNIZER
#define __PACE2024__OUTERPLANAR_RECOGNIZER

#include "graph.h"
#include "recognizer.h"

namespace banana {
namespace recognizer {

class OuterplanarRecognizer : public Recognizer<graph::Graph>
{
public:
  OuterplanarRecognizer(graph::Graph &graph);
  ~OuterplanarRecognizer() override = default;

  bool check() override;
};

} // namespace recognizer
} // namespace banana

#endif // __PACE2024__OUTERPLANAR_RECOGNIZER
