/******************************************************************************
 * Top contributors (to current version):
 *   Alan Prado, Kaio Vieira, Laila, Guilherme Gomes
 *
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 *
 * Basic Graph class
 */

#ifndef __PACE2024__GRAPH_HPP
#define __PACE2024__GRAPH_HPP

#include <vector>

/**
 * The Graph class.
 * The fundamental building block of life, the Universe and everything.
 */

template <class T> // Define, someday, a Vertex class? Rita? 👀
class Graph {
 // TO-DO: Perhaps use a hashmap to store the vertices in the graph?
 protected:
  typedef std::pair<T, T> Edge;
  typedef std::vector<Edge> ContractionSequence;

  std::vector<std::vector<bool>> m_graph;
  std::vector<unsigned> m_degrees;
  std::vector<unsigned> m_labels;
  int m_edgeCount;

 public:
  Graph(int n);
  Graph(const Graph& H);  // Copy constructor
  ~Graph() = default;

  /** Adds edge (u, v) to graph */
  void addEdge(T u, T v);
  /** Removes edge (u, v) from graph */
  void removeEdge(T u, T v);
  /** Checks if graph has edge (u, v) */
  bool hasEdge(T u, T v) const;
  /** Get the number of edges in the graph */
  unsigned countEdges() const;
  /** Get the number of vertices in the graph */
  unsigned countVertices() const;
  /** Get the neighbors of vertex 'u' */
  std::vector<T> neighborhood(T u) const;
  /**
   * Returns a vector corresponding to the symmetric difference between
   * vertices 'u' and 'v'.
   *
   * The symmetric difference of two vertices is defined as the set of vertices
   * which are neighbors of either 'u' or 'v', but not both.
   */
  std::vector<T> symmetricDifference(T u, T v) const;
  /** Get the adjacency list representation of the graph. */
  std::vector<std::vector<T>> adjacencyList() const;
  /** Edge set of the graph. */
  std::vector<Edge> edges() const;
  /** Get the degree of vertex 'u'. */
  unsigned degree(T u) const;
  /** Get the degree vector of the graph. */
  std::vector<unsigned> degree() const;
  /** Get the label corresponding to a vertex 'u' */
  int label(T u) const;
  /** Checks if the graph is connected */
  bool isConnected() const;
  /** Checks if the graph is a tree */
  bool isTree() const;
  /** Checks if the graph is bipartite */
  bool isBipartite() const;
  /**
   * Creates the complement (inverse) of the graph.
   *
   * The complement is a graph on the same verties such that two distinct
   * vertices are adjacent if and only if they are not adjacent in the original
   * graph.
   */
  Graph complement() const;
  std::vector<Graph> subgraphs(std::vector<std::vector<int>> comp) const;
  Graph quotient(std::vector<std::vector<int>>& partition) const;
  std::vector<std::vector<int>> refine(std::vector<std::vector<int>> &partition, std::vector<int> pivot) const;
  std::vector<std::vector<int>> modular_partition(std::vector<std::vector<int>> &P) const;
  std::vector<std::vector<int>> prime_decomposition() const;
  std::pair<std::vector<Graph>, std::vector<std::pair<int, std::vector<int>>>> decompose() const;
  void decompose(std::vector<Graph>& g,  std::vector<std::pair<int, std::vector<int>>>& modular_tree, int parent) const;
  ContractionSequence recompose(std::vector<std::pair<ContractionSequence, int>>& seq, std::vector<std::pair<int, std::vector<int>>>& modular_tree) const;
  int width(const ContractionSequence& seq) const;
	std::pair<int, std::vector<Edge>> greedy_upper_bound() const;
	int greedy_lower_bound() const;
};

#endif  // __PACE2024__GRAPH_HPP
