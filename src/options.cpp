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

#include "options.h"

#include <cstdint>
#include <cstring>
#include <filesystem>
#include <getopt.h>
#include <stdexcept>
#include <cassert>

namespace banana {
namespace options {

void resolve_sub_solver(HolderIP &ip, const std::string &name)
{
  assert(ip.solverMode == IPSolverMode::OR_TOOLS);

  if (name == "highs")
  {
    ip.subSolverMode = IPSubSolverMode::HIGHS;
  }
  else if (name == "glpk")
  {
    ip.subSolverMode = IPSubSolverMode::GLPK;
  }
  else if (name == "cp_mip")
  {
    ip.subSolverMode = IPSubSolverMode::CP_MIP;
  }
  else if (name == "cp_sat")
  {
    ip.subSolverMode = IPSubSolverMode::CP_SAT;
  }
  else if (name == "cbc")
  {
    ip.subSolverMode = IPSubSolverMode::CBC;
  }
  else
  {
    throw std::invalid_argument("Invalid OR-Tools Sub Solver: " + name);
  }
}

void Options::parseArguments(int argc, char *argv[])
{

  int opt;

  struct option longopts[] = {
      /** IP options */
      {"ipsolver", required_argument, nullptr,
       static_cast<uint32_t>(Flags::IPSolverMode)},
      {"ipformulation", required_argument, nullptr,
       static_cast<uint32_t>(Flags::IPFormulation)},
      {"ipprefixconstraints", required_argument, nullptr,
       static_cast<uint32_t>(Flags::IPPrefixConstraints)},
      /** Verification options */
      {"verify", required_argument, nullptr,
       static_cast<uint32_t>(Flags::VerifyMode)},
      {0}};

  while ((opt = getopt_long(argc, argv, "", longopts, 0)) != -1)
  {
    const std::string or_tools_prefix = "or-tools:";
    auto optarg_s = std::string{optarg};
    switch (opt)
    {
    /** IP options */
    case static_cast<uint32_t>(Flags::IPSolverMode):
      if (!strcmp(optarg, "lpsolve"))
      {
        ip.solverMode = IPSolverMode::LPSOLVE;
      }
      else if (!strcmp(optarg, "gurobi"))
      {
        ip.solverMode = IPSolverMode::GUROBI;
      }
      else if (optarg_s.rfind(or_tools_prefix) == 0)
      {
        std::string solver_name = optarg_s.substr(or_tools_prefix.length());
        ip.solverMode = IPSolverMode::OR_TOOLS;
        resolve_sub_solver(ip, solver_name);
      }
      else
      {
        throw std::invalid_argument("Invalid IP Solver: " +
                                    std::string{optarg});
      }
      break;
    case static_cast<uint32_t>(Flags::IPFormulation):
      if (!strcmp(optarg, "simple"))
      {
        ip.formulation = IPFormulation::SIMPLE;
      }
      else if (!strcmp(optarg, "shorter"))
      {
        ip.formulation = IPFormulation::SHORTER;
      }
      else if (!strcmp(optarg, "quadratic"))
      {
        ip.formulation = IPFormulation::QUADRATIC;
      }
      else if (!strcmp(optarg, "vini"))
      {
        ip.formulation = IPFormulation::VINI;
      }
      else
      {
        throw std::invalid_argument("Invalid IP Formulation: " +
                                    std::string{optarg});
      }
      break;
    case static_cast<uint32_t>(Flags::IPPrefixConstraints):
      if (!strcmp(optarg, "none"))
      {
        ip.prefixConstraints = IPPrefixConstraints::NONE;
      }
      else if (!strcmp(optarg, "x"))
      {
        ip.prefixConstraints = IPPrefixConstraints::X;
      }
      else if (!strcmp(optarg, "y"))
      {
        ip.prefixConstraints = IPPrefixConstraints::Y;
      }
      else if (!strcmp(optarg, "both"))
      {
        ip.prefixConstraints = IPPrefixConstraints::BOTH;
      }
      else
      {
        throw std::invalid_argument("Invalid IP Prefix Constraint: " +
                                    std::string{optarg});
      }
      break;
    /** Verify options */
    case static_cast<uint32_t>(Flags::VerifyMode):
      verify.verifyMode = VerifyMode::COMPLETE;
      verify.verifyPath = optarg;
      if (!std::filesystem::exists(verify.verifyPath))
      {
        throw std::invalid_argument("Invalid Verification Path: " +
                                    std::string{optarg});
      }
      break;
    /** IP Heuristics Integration option */
    case static_cast<uint32_t>(Flags::IPHeuristicMode):
      ip.heuristicMode = IPHeuristicMode::ON;
      break;
    default:
      throw std::invalid_argument("Invalid option");
    }
  }
}

} // namespace options
} // namespace banana
