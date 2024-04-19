/******************************************************************************
 * Top contributors (to current version):
 *   Alan Prado, Kaio Vieira
 *
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 *
 * Basic graph class.
 */

#ifndef __PACE2024__GRAPH_HPP
#define __PACE2024__GRAPH_HPP

#include "abstract_graph.h"

#include <memory>
#include <vector>

namespace banana {
namespace graph {

/**
 * Basic graph class.
 */
class Graph : public TGraph<int>
{
 protected:
  std::vector<std::vector<bool>> m_adjacencyMatrix;
  std::vector<unsigned> m_vertexDegrees;
  std::vector<int> m_vertexLabels;
  unsigned m_edgeCount;

 public:
  Graph(int n);
  Graph(const Graph& H);  // Copy constructor
  ~Graph() = default;

  /** Adds edge (u, v) to graph */
  void addEdge(int u, int v) override;
  /** Removes edge (u, v) from graph */
  void removeEdge(int u, int v) override;
  /** Checks if graph has edge (u, v) */
  bool hasEdge(int u, int v) const override;
  /** Get the number of edges in the graph */
  unsigned countEdges() const override;
  /** Edge set of the graph. */
  std::vector<Edge> edges() const override;

  /** Get the number of vertices in the graph */
  unsigned countVertices() const override;
  /** Get the neighbors of vertex 'u' */
  std::vector<int> neighborhood(int u) const override;
  /**
   * Returns a vector corresponding to the symmetric difference between
   * vertices 'u' and 'v'.
   *
   * The symmetric difference of two vertices is defined as the set of vertices
   * which are neighbors of either 'u' or 'v', but not both.
   */
  std::vector<int> symmetricDifference(int u, int v) const override;
  /** Get the degree of vertex 'u'. */
  unsigned degree(int u) const override;
  /** Get the degree vector of the graph. */
  std::vector<unsigned> degree() const override;
  /** Get the label corresponding to a vertex 'u' */
  unsigned label(int u) const override;

  /** Checks if the graph is connected */
  bool isConnected() const override;
  /** Checks if the graph is a tree */
  bool isTree() const override;
  /** Checks if the graph is bipartite */
  bool isBipartite() const override;

  /**
   * Creates the complement (inverse) of the graph.
   *
   * The complement is a graph on the same verties such that two distinct
   * vertices are adjacent if and only if they are not adjacent in the original
   * graph.
   */
  std::unique_ptr<TGraph> complement() const override;
  /** Divides a graph into disjoint subgraphs */
  std::vector<std::unique_ptr<TGraph>>
  disjointSubgraphs(std::vector<std::vector<int>>& subsets) const override;
  /**
   * Creates the quotient graph from 'partition'.
   *
   * The quotient graph vertices are the blocks of 'partition', and a block is
   * adjacent to another if some vertex in it is adjacent to some vertex in the
   * other with respect to the edge set of the original graph.
   */
  std::unique_ptr<TGraph>
  quotient(std::vector<std::vector<int>>& partition) const override;

  /** TO-DO */
  std::vector<std::vector<int>>
  modularPartition(std::vector<std::vector<int>>& partition) const override;
  /** TO-DO */
  std::vector<std::vector<int>> primeDecomposition() const override;

  /** Get the adjacency list representation of the graph. */
  std::vector<std::vector<int>> adjacencyList() const override;
};

} // namespace graph
} // namespace banana

#endif  // __PACE2024__GRAPH_HPP
