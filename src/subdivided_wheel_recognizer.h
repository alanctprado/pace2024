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
 * Subdivided Wheel Recognizer
 */

#ifndef __PACE2024__SUBDIVIDEDWHEEL_RECOGNIZER
#define __PACE2024__SUBDIVIDEDWHEEL_RECOGNIZER

#include "graph.h"
#include "recognizer.h"

namespace banana {
namespace recognizer {

class SubdividedWheelRecognizer : public Recognizer<graph::Graph>
{
public:
  SubdividedWheelRecognizer(graph::Graph &graph);
  ~SubdividedWheelRecognizer() override = default;

  bool check() override;
};

} // namespace recognizer
} // namespace banana

#endif // __PACE2024_SUBDIVIDEDWHEEL_RECOGNIZER
