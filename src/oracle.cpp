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
#include <set>
#include <algorithm>
#include <iostream>

namespace banana {
namespace solver {

Oracle::Oracle() : m_graph(0, 0), m_crossing_matrix(m_graph) {}

unsigned Oracle::countVerticesA() const {
  return m_graph.countVerticesA();
}

Oracle::Oracle(const graph::BipartiteGraph G) : m_graph(G), m_crossing_matrix(G)
{
    m_graph.sortAdjacencyList();
}

std::vector<int> Oracle::neighborhood(int b_vertex) const {
    return m_graph.neighborhood(m_graph.countVerticesA() + b_vertex);
}

int Oracle::degree(int b_vertex) const {
  return m_graph.degree(m_graph.countVerticesA() + b_vertex);
}

std::pair<int, int> Oracle::getInterval(int b_vertex) const {
    const auto &neighbors = neighborhood(b_vertex);
    if (neighbors.empty()) return {1, 1};
    return {neighbors.front(), neighbors.back()};
}

std::vector<std::pair<int, int>> Oracle::getIntervals(std::vector<int> b_vertices) const {
    std::vector<std::pair<int, int>> intervals;
    for (int b : b_vertices)
        intervals.push_back(getInterval(b));
    return intervals;
}

std::vector<std::pair<int, int>> Oracle::getIntervals(SubProblem b_vertices) const {
    std::vector<std::pair<int, int>> intervals;
    for (auto b : b_vertices)
        intervals.push_back(getInterval(b.first));
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
  std::unordered_map<int, int> position;
  std::unordered_map<int, F> weight;
  for (int i = 0; i < (int)order.size(); i++)
  {
    position[order[i].first] = i;
    weight[order[i].first] = order[i].second;
  }

  /** How to get the edges? */
  std::vector<std::pair<int, int>> edges;
  for (auto [v, w] : order) 
  {
    auto neighbors = neighborhood(v);
    assert(neighbors.size() % w.den() == 0);
    for (int i = 0; i < (int)neighbors.size(); i += w.den()) 
    {
      edges.emplace_back(neighbors[i], v);
    }
  }

  std::sort(edges.begin(), edges.end(), [&](auto edge1, auto edge2) {
    auto [a1, b1] = edge1;
    auto [a2, b2] = edge2;
    if (b1 == b2)
      return a1 < a2;
    return position[b1] < position[b2];
  });

  /** TODO: Declare FwTree global? */
  int crossings = 0;
  library::FenwickTree<int> tree(nA);

  for (int l = 0, r = 0; l < (int)edges.size(); l = r)
  {
    while (r < (int)edges.size() && edges[l].second == edges[r].second)
      r++;

    for (int i = l; i < r; i++)
    {
      auto [v_a, v_b] = edges[i];
      crossings += tree.suffixQuery(v_a + 1) * weight[v_b].num();
    }

    for (int i = l; i < r; i++)
    {
      auto [v_a, v_b] = edges[i];

      tree.update(v_a, weight[v_b].num());
    }
  }

  return crossings;
}

bool Oracle::verify(const std::vector<Vertex> &order,
                           int expected_crossings) const
{
  //assert(order.size() == m_graph.countVerticesA());
  //std::cout << "Order size: " << order.size() << "\n";
  //for (auto [v, w] : order)
  //{
  //  std::cout << v << " " << w.num() << "/" << w.den() << std::endl;
  //}
  //std::cout << "Expected " << expected_crossings << " crossings" << std::endl;
  //std::cout << "Actual " << numberOfCrossings(order) << " crossings" << std::endl;
  return numberOfCrossings(order) == expected_crossings;
}

// get all orientable pairs of some subinstance P
// works in O(P log P)
std::vector<std::pair<int, int>> Oracle::getOrientablePairs(const std::vector<int> &p) const {
  enum State { FINISH, START };
  std::vector<std::tuple<int, State, int>> events; // coordinate, (termino/inicio), vertice
  for (auto vertex : p) {
    auto [l, r] = getInterval(vertex);
    events.emplace_back(l, START, vertex);
    events.emplace_back(r, FINISH, vertex);
  }
  std::sort(begin(events), end(events));
  std::vector<std::pair<int, int>> answer;
  std::set<int> alive;
  for (auto [coord, type, vertex] : events) {
      if (type == FINISH) {
          alive.erase(vertex);
      } else {
        for (auto v : alive) answer.emplace_back(vertex, v);
        alive.insert(vertex);
      }
  }
  return answer;
}

std::vector<std::pair<int, int>> Oracle::getOrientablePairs(const SubProblem &p) const {
  enum State { FINISH, START };
  std::vector<std::tuple<int, State, int>> events; // coordinate, (termino/inicio), vertice
  for (int i = 0; i < p.size(); ++i) {
    auto [l, r] = getInterval(p[i].first);
    events.emplace_back(l, START, i);
    events.emplace_back(r, FINISH, i);
  }
  std::sort(begin(events), end(events));
  std::vector<std::pair<int, int>> answer;
  std::set<int> alive;
  for (auto [coord, type, vertex] : events) {
      if (type == FINISH) {
          alive.erase(vertex);
      } else {
        for (auto v : alive) answer.emplace_back(vertex, v);
        alive.insert(vertex);
      }
  }
  return answer;
}

std::vector<std::pair<int, int>> Oracle::getCompressedIntervals(const SubProblem& instance) const {

  std::vector<int> c;

  std::vector<std::pair<int, int>> intervals = getIntervals(instance);

  for (auto [l, r] : intervals) c.push_back(l), c.push_back(r);

  std::sort(begin(c), end(c));
  
  c.erase(unique(begin(c), end(c)), end(c)); 

  for (auto& [l, r] : intervals) {
      l = std::lower_bound(begin(c), end(c), l) - begin(c);
      r = std::lower_bound(begin(c), end(c), r) - begin(c);
  }

  return intervals;
}

// TODO: REMOVE or PUT IN A APPROPIATE PLACE
// write the << operator for Oracle::SubProblem
std::ostream &operator<<(std::ostream &os, const Oracle::SubProblem &instance)
{
  os << "SubProblem: ";
  for (auto [vertex, weight] : instance)
  {
    os << vertex << " ";
  }
  os << "\n";
  return os;
}

} // Namespace solver
} // namespace banana

// Explain what this code does
