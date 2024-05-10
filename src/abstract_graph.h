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
 * Definition of an abstract graph class
 */

#ifndef __PACE2024__ABSTRACT_GRAPH_HPP
#define __PACE2024__ABSTRACT_GRAPH_HPP

#include <memory>
#include <vector>

namespace banana {
namespace graph {

/**
 * Abstract graph class.
 *
 * The fundamental building block of life, the Universe and everything.
 */
template <class V, class E> // Define, someday, a Vertex class? Rita? 👀
class TGraph
{
  protected:
    typedef V Vertex;
    typedef E Edge;
public:
  TGraph() {}
  virtual ~TGraph() {}

  /** Adds edge e to graph */
  virtual void addEdge(Vertex v, Edge e) = 0;
  /** Removes edge e from graph */
  virtual void removeEdge(Vertex v, Edge e) = 0;
  /** Checks if graph has edge e */
  virtual bool hasEdge(Vertex v, Edge e) const = 0;
  /** Get the number of edges in the graph */
  virtual unsigned countEdges() const = 0;
  /** Edge set of the graph. */
  virtual std::vector<std::pair<Vertex, Edge>> edges() const = 0;

  /** Get the number of vertices in the graph */
  virtual unsigned countVertices() const = 0;
  /** Get the neighbors of vertex 'u' */
  virtual std::vector<Edge> neighborhood(Vertex u) const = 0;
  /**
   * Returns a vector corresponding to the symmetric difference between
   * vertices 'u' and 'v'.
   *
   * The symmetric difference of two vertices is defined as the set of vertices
   * which are neighbors of either 'u' or 'v', but not both.
   */
  virtual std::vector<Edge> symmetricDifference(Vertex u, Vertex v) const = 0;
  /** Get the degree of vertex 'u'. */
  virtual unsigned degree(Vertex u) const = 0;
  /** Get the degree vector of the graph. */
  virtual std::vector<unsigned> degree() const = 0;
  /** Get the label corresponding to a vertex 'u' */
  virtual unsigned label(Vertex u) const = 0;

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
  virtual std::unique_ptr<TGraph> complement() const = 0;
  /** Divides a graph into disjoint subgraphs */
  virtual std::vector<std::unique_ptr<TGraph>>
  disjointSubgraphs(std::vector<std::vector<Edge>> &subsets) const = 0;
  /**
   * Creates the quotient graph from 'partition'.
   *
   * The quotient graph vertices are the blocks of 'partition', and a block is
   * adjacent to another if some vertex in it is adjacent to some vertex in the
   * other with respect to the edge set of the original graph.
   */
  virtual std::unique_ptr<TGraph>
  quotient(std::vector<std::vector<Edge>> &partition) const = 0;
  /** TO-DO */
  virtual std::vector<std::vector<Edge>>
  modularPartition(std::vector<std::vector<Edge>> &P) const = 0;
  /** TO-DO */
  virtual std::vector<std::vector<Edge>> primeDecomposition() const = 0;

  /** Get the adjacency list representation of the graph. */
  virtual std::vector<std::vector<Edge>> adjacencyList() const = 0;
};

} // namespace graph
} // namespace banana

#endif // __PACE2024__ABSTRACT_GRAPH_HPP
