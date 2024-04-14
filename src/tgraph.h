/******************************************************************************
 * Top contributors (to current version):
 *   Alan Prado, Kaio Vieira, Laila Lopes, Guilherme Gomes
 *
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 *
 * Definition of an abstract graph class
 */

#ifndef __PACE2024__TGRAPH_HPP
#define __PACE2024__TGRAPH_HPP

#include <vector>

/**
 * Abstract graph class.
 * The fundamental building block of life, the Universe and everything.
 */

template <class T> // Define, someday, a Vertex class? Rita? 👀
class TGraph {
 protected:
  typedef std::pair<T, T> Edge;

 public:
  TGraph(int n) {}
  TGraph(const TGraph& H) {} // Copy constructor
  virtual ~TGraph() {}

  /** Adds edge (u, v) to graph */
  virtual void addEdge(T u, T v) = 0;
  /** Removes edge (u, v) from graph */
  virtual void removeEdge(T u, T v) = 0;
  /** Checks if graph has edge (u, v) */
  virtual bool hasEdge(T u, T v) const = 0;
  /** Get the number of edges in the graph */
  virtual unsigned countEdges() const = 0;
  /** Get the number of vertices in the graph */
  virtual unsigned countVertices() const = 0;
  /** Get the neighbors of vertex 'u' */
  virtual std::vector<T> neighborhood(T u) const = 0;
  /**
   * Returns a vector corresponding to the symmetric difference between
   * vertices 'u' and 'v'.
   *
   * The symmetric difference of two vertices is defined as the set of vertices
   * which are neighbors of either 'u' or 'v', but not both.
   */
  virtual std::vector<T> symmetricDifference(T u, T v) const = 0;
  /** Get the adjacency list representation of the graph. */
  virtual std::vector<std::vector<T>> adjacencyList() const = 0;
  /** Edge set of the graph. */
  virtual std::vector<Edge> edges() const = 0;
  /** Get the degree of vertex 'u'. */
  virtual unsigned degree(T u) const = 0;
  /** Get the degree vector of the graph. */
  virtual std::vector<unsigned> degree() const = 0;
  /** Get the label corresponding to a vertex 'u' */
  virtual unsigned label(T u) const = 0;
  /** Checks if the graph is connected */
  virtual bool isConnected() const = 0;
  /** Checks if the graph is a tree */
  virtual bool isTree() const = 0;
  /** Checks if the graph is bipartite */
  virtual bool isBipartite() const = 0;
  /**
   * Creates the complement (inverse) of the graph.
   *
   * The complement is a graph on the same verties such that two distinct
   * vertices are adjacent if and only if they are not adjacent in the original
   * graph.
   */
  virtual TGraph<T> complement() const = 0;
  /** Divides a graph into disjoint subgraphs */
  virtual std::vector<TGraph<T>>
    disjointSubgraphs(std::vector<std::vector<T>> subsets) const = 0;
  /**
   * Creates the quotient graph from 'partition'.
   *
   * The quotient graph vertices are the blocks of 'partition', and a block is
   * adjacent to another if some vertex in it is adjacent to some vertex in the
   * other with respect to the edge set of the original graph.
   */
  virtual TGraph<T> quotient(std::vector<std::vector<T>>& partition) const;
  /** TO-DO */
  virtual std::vector<std::vector<int>>
  modularPartition(std::vector<std::vector<int>> &P) const = 0;
  /** TO-DO */
  virtual std::vector<std::vector<int>> primeDecomposition() const = 0;
};

#endif  // __PACE2024__GRAPH_HPP
