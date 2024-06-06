/******************************************************************************
 * Top contributors (to current version):
 *   Heitor Leite 
 *
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 *
 * Dynamic programming solver.
 */

#include "oracle.h"
#include "dp_solver.h"
#include "environment.h"
#include <algorithm>
#include <cmath>
#include <limits>

#include <iostream>
#include <bitset>
#include <stdexcept>

namespace banana {
namespace solver {
namespace dp {

DPSolver::DPSolver(Oracle::SubProblem G) : MetaSolver(G) {
  decompose();
}

/* Computes the nice path-like decomposition */
void DPSolver::decompose() {

  const auto& m_oracle = Environment::oracle();
  auto intervals = m_oracle.getIntervals(m_instance);

  std::vector<std::tuple<int, Event, int>> events;
  for (int i = 0; i < m_instance.size(); ++i) {
    auto [l, r] = intervals[i];
    events.emplace_back(l, INSERT, i);
    events.emplace_back(r, FORGET, i);
  }
  std::sort(begin(events), end(events));

  int cur = 0;
  for (auto[t, e, v] : events) {
    cur += (e == INSERT ? 1 : -1);
    m_decomposition.push_back({e, v});
    m_bag_sizes.push_back(cur);
  }
}

/* We use double because it goes to inf instead of overflowing */
double DPSolver::estimateTime() {
  double est = 0;
  for (auto b : m_bag_sizes)
    est += std::pow(2, b) * b;
  return est;
}

double DPSolver::estimateMemory() {
  double est = 0;
  for (auto b : m_bag_sizes)
    est += std::pow(2, b);
  return est * sizeof(int);
}

int DPSolver::solve() {
  int n = m_decomposition.size();
  const auto& oracle = Environment::oracle();

  std::vector<std::vector<int>> dp(n+1), opt(n+1);
  std::vector<std::vector<Oracle::Vertex>> bags;
  std::vector<Oracle::Vertex> bag;
  std::vector<int> c;

  dp[0].assign(1, 0);
  opt[0].assign(1, -1);
  bags.push_back({});

  auto get_pos = [](std::vector<Oracle::Vertex> bag, int v) -> int {
    for (int i = 0; i < bag.size(); i++)
      if (bag[i].first == v) return i;
    throw std::runtime_error("vertex not found in bag!");
  };

  for (int i = 1; i <= n; i++) {
    auto[e, vi] = m_decomposition[i-1];
    auto v = m_instance[vi];

    int j2 = -1;

    if (e == INSERT) bag.push_back(v);
    else {
      j2 = std::find(bag.begin(), bag.end(), v)-bag.begin();
      bag.erase(bag.begin()+j2);
      c.erase(c.begin()+j2);
    }

    int s = bag.size();
    dp[i].assign(1<<s, std::numeric_limits<int>::max());
    opt[i].assign(1<<s, -1);

    if (e == FORGET)
    {
      for (int j = 0; j < s; j++)
        c[j] += oracle.getCrossings(v, bag[j]);

      int msk = (1<<j2);
      for (int msk1 = 0, msk2 = msk; msk1 < (1<<s); msk1++, msk2 = (msk2+1)|msk)
      {
        dp[i][msk1] = dp[i-1][msk2];
      }
    }
    else // INSERT
    {
      c.push_back(0);

      for (int msk = 0; msk < (1<<(s-1)); msk++)
      {
        dp[i][msk] = dp[i-1][msk];
      }

      for (int msk = (1<<(s-1)); msk < (1<<s); msk++)
      {
        for (int j = 0; j < s; j++) if ((msk>>j)&1)
        {
          int c2 = 0;
          for (int k = 0; k < s; k++)
          {
            if (k == j || !((msk>>k)&1)) continue;
            c2 += oracle.getCrossings(bag[k], bag[j]);
          }
          
          if (dp[i][msk-(1<<j)] + c[j] + c2 < dp[i][msk])
          {
            dp[i][msk] = dp[i][msk-(1<<j)] + c[j] + c2;
            opt[i][msk] = j;
          }
        }
      }
    }
     
    bags.push_back(bag);
  }
  
  int msk = 0;
  for (int i = n; i >= 1; i--) {
    auto[e, vi] = m_decomposition[i-1];
    auto v = m_instance[vi];
    
    if (e == FORGET)
    {   
      int p = get_pos(bags[i-1], v.first);
      int m1 = (1<<p)-1, m2 = ~m1;
      msk = ((msk&m2)<<1) | (1<<p) | (msk&m1);
    }
    else // INSERT
    {
      while (opt[i][msk] != -1) {
        m_order.push_back(bags[i][opt[i][msk]]);
        msk &= ~(1<<opt[i][msk]);
      }
    }
  }

  std::reverse(m_order.begin(), m_order.end());
  return dp[n][0];
}

} // namespace dp
} // namespace solver
} // namespace banana
