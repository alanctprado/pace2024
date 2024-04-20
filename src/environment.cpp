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
 * Global environment
 */

#include "environment.h"
#include "options.h"

namespace banana {

void Environment::setOptions(int argc, char* argv[])
{
  m_options.parseArguments(argc, argv);
}

options::Options Environment::options()
{
  return m_options;
}

} // namespace banana
