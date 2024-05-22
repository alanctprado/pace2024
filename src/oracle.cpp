/******************************************************************************
 * Top contributors (to current version):
 *   Kaio Vieira, Gabriel Ubiratan
 *
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 *
 * Data Structure to store and index information about the instance being solved
 */
#include "oracle.h"
#include "bipartite_graph.h"
#include "fenwick_tree.h"
#include <cassert>

namespace banana {
namespace solver {

Oracle::Oracle() : m_graph(0, 0), m_crossing_matrix(m_graph) {}

Oracle::Oracle(const graph::BipartiteGraph G) : m_graph(G), m_crossing_matrix(G)
{
    m_graph.sortAdjacencyList();
}

std::vector<int> Oracle::neighborhood(int b_vertex) const {
    return m_graph.neighborhood(m_graph.countVerticesA() + b_vertex);
}

std::pair<int, int> Oracle::getInterval(int b_vertex) const {
    const auto &neighbors = neighborhood(b_vertex);
    return {neighbors.front(), neighbors.back()};
}

std::vector<std::pair<int, int>> Oracle::getIntervals(std::vector<int> b_vertices) const {
    std::vector<std::pair<int, int>> intervals;
    for (int b : b_vertices)
        intervals.push_back(getInterval(b));
    return intervals;
}

/** TODO: Verify how to use crossing_matrix */
int Oracle::getCrossings(Vertex v_i, Vertex v_j) const {
    auto &[i, w_i] = v_i;
    auto &[j, w_j] = v_j;
    F answer = w_i * w_j * F(m_crossing_matrix(i, j));
    assert(answer.den() == 1);
    return answer.num();
}

int Oracle::numberOfCrossings(const std::vector<Vertex> &order) const
{
  int nA = m_graph.countVerticesA();
  int nB = m_graph.countVerticesB();

  /** Use a hashmap? */
  std::vector<int> position(nB);
  for (int i = 0; i < order.size(); i++)
  {
    position[order[i] - nA] = i;
  }

  /** How to get the edges? */
  auto edges = m_graph.edges();
  std::sort(edges.begin(), edges.end(), [&](auto edge1, auto edge2) {
    auto [a1, b1] = edge1;
    auto [a2, b2] = edge2;
    if (a1 > b1)
      std::swap(a1, b1);
    if (a2 > b2)
      std::swap(a2, b2);

    if (b1 == b2)
      return a1 < a2;
    return position[b1 - nA] < position[b2 - nA];
  });

  /** Declare FwTree global? */
  int crossings = 0;
  library::FenwickTree<int> tree(nA);

  for (int l = 0, r = 0; l < (int)edges.size(); l = r)
  {
    while (r < (int)edges.size() && edges[l].second == edges[r].second)
      r++;

    for (int i = l; i < r; i++)
    {
      auto [v_a, v_b] = edges[i];
      if (v_a > v_b)
        std::swap(v_a, v_b);

      /** multiply by the vertex weight, and assert it is an integer */
      crossings += tree.suffixQuery(v_a + 1);
    }

    for (int i = l; i < r; i++)
    {
      auto [v_a, v_b] = edges[i];
      if (v_a > v_b)
        std::swap(v_a, v_b);

      /** add 1 or weight? weight! */
      tree.update(v_a, +1);
    }
  }

  return crossings;
}

bool Oracle::verify(const std::vector<Vertex> &order,
                           int expected_crossings) const
{
  assert(order.size() == m_graph.countVerticesA());
  return numberOfCrossings(order) == expected_crossings;
}

} // Namespace solver
} // namespace banana

// Explain what this code does
