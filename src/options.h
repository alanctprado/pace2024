/******************************************************************************
 * Top contributors (to current version):
 *   Alan Prado, Luis Higino
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
  IPFormulation,
  IPPrefixConstraints,
  /** Verify options */
  VerifyMode
};

enum class IPSubSolverMode
{
  NONE,
  HIGHS,
  CBC,
  CP_MIP,
  CP_SAT,
  GLPK,
};

enum class IPSolverMode
{
  LPSOLVE,
  GUROBI,
  OR_TOOLS,

  //  HIGHS,
  //  COINOR,
  //  GLPK,
  __MAX_VALUE = LPSOLVE
};

enum class IPFormulation
{
  SIMPLE,
  SHORTER,
  QUADRATIC,
  VINI,
  __MAX_VALUE = SHORTER
};

enum class IPPrefixConstraints
{
  NONE,
  X,
  Y,
  BOTH,
  __MAX_VALUE = NONE
};

enum class VerifyMode
{
  LIGHT,
  COMPLETE,
  __MAX_VALUE = LIGHT
};

struct HolderIP
{
  IPSolverMode solverMode = IPSolverMode::LPSOLVE;
  IPSubSolverMode subSolverMode = IPSubSolverMode::NONE;
  IPFormulation formulation = IPFormulation::SHORTER;
  IPPrefixConstraints prefixConstraints = IPPrefixConstraints::NONE;
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
  void parseArguments(int argc, char *argv[]);
  ~Options() = default;

  HolderIP ip;
  HolderVerify verify;
};

} // namespace banana::options

#endif // __PACE2024__OPTIONS_HPP
