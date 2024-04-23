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

#ifndef __PACE2024__OPTIONS_HPP
#define __PACE2024__OPTIONS_HPP

#include <string>

namespace banana::options {

enum class Flags
{
  /** IP options */
  IPSolverMode,
  /** Verify options */
  VerifyMode
};

enum class IPSolverMode
{
  LPSOLVE, HIGHS, COINOR, GLPK,
  __MAX_VALUE = LPSOLVE
};

enum class VerifyMode
{
  LIGHT, FULL,
  __MAX_VALUE = LIGHT
};

struct HolderIP
{
  IPSolverMode solverMode = IPSolverMode::LPSOLVE;
};

struct HolderVerify
{
  VerifyMode verifyMode = VerifyMode::LIGHT;
  std::string verifyPath = "";
};

class Options
{
 protected:

 public:
  Options() = default;
  void parseArguments(int argc, char* argv[]);
  ~Options() = default;

  HolderIP h_ip;
  HolderVerify h_v;
};

} // namespace banana::options

#endif  // __PACE2024__OPTIONS_HPP
