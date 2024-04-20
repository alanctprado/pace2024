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
 * Contains code for handling command-line options.
 */

#include "options.h"

#include <cstdint>
#include <cstring>
#include <getopt.h>
#include <stdexcept>

namespace banana::options {

void Options::parseArguments(int argc, char* argv[])
{
  int opt;

  struct option longopts[] = {
    { "ipsolver", required_argument, nullptr,
      static_cast<uint32_t>(IPSolverMode()) },
    { 0 }
  };

  while ((opt = getopt_long (argc, argv, "", longopts, 0)) != -1)
  {
    switch (opt)
    {
      case static_cast<uint32_t>(IPSolverMode()):
        if (!strcmp(optarg, "lpsolve"))
        {
          ip.solverMode = IPSolverMode::LPSOLVE;
        }
        else
        {
          throw std::invalid_argument("Invalid IP Solver: " + std::string{optarg});
        }
        break;
      default:
        throw std::invalid_argument("Invalid option");
        break;
    }
  }
}

} // namespace banana::options
