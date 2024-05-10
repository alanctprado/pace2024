/******************************************************************************
 * Top contributors (to current version):
 *   Gabriel Lucas
 *
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 *
 * Basic graph class with adjacency list representation.
 */

#include "lgraph.h"

#include <algorithm>
#include <cassert>
#include <numeric>

namespace banana {
namespace graph {

LGraph::LGraph(int n) : TGraph<Vertex, Edge>()
{
  m_adjacencyList.assign(n, std::vector<Edge>());
  m_vertexDegrees.assign(n, 0);
  std::iota(m_vertexLabels.begin(), m_vertexLabels.end(), 0);
  m_edgeCount = 0;
}

LGraph::LGraph(const LGraph &H)
{
  m_adjacencyList = H.m_adjacencyList;
  m_vertexDegrees = H.m_vertexDegrees;
  m_vertexLabels = H.m_vertexLabels;
  m_edgeCount = H.m_edgeCount;
}

void LGraph::addEdge(Vertex v, Edge e)
{
  assert(v != e);
  m_adjacencyList[v].push_back(e);
  m_adjacencyList[e].push_back(v);
  m_vertexDegrees[v]++, m_vertexDegrees[e]++;
  m_edgeCount++;
}

void LGraph::removeEdge(Vertex v, Edge e)
{
  assert(e != v);
  auto it1 = std::find(begin(m_adjacencyList[v]), end(m_adjacencyList[v]), e);

  if (it1 != end(m_adjacencyList[v]))
  {
    m_adjacencyList[v].erase(it1);
  }

  auto it2 = std::find(begin(m_adjacencyList[e]), end(m_adjacencyList[e]), v);

  if (it2 != end(m_adjacencyList[e]))
  {
    m_adjacencyList[e].erase(it2);
  }

  m_vertexDegrees[v]--, m_vertexDegrees[e]--;
  m_edgeCount--;
}

bool LGraph::hasEdge(Vertex v, Edge e) const
{
  assert(v != e);
  return std::find(begin(m_adjacencyList[v]), end(m_adjacencyList[v]), e) !=
         end(m_adjacencyList[v]);
}

unsigned LGraph::countEdges() const { return m_edgeCount; }

unsigned LGraph::countVertices() const
{
  return (unsigned)m_adjacencyList.size();
}

std::vector<LGraph::Edge> LGraph::neighborhood(Vertex v) const
{
  return m_adjacencyList[v];
}

std::vector<LGraph::Edge> LGraph::symmetricDifference(Vertex v, Vertex u) const
{
  throw std::runtime_error("Not implemented :(");
}

std::vector<std::pair<LGraph::Vertex, LGraph::Edge>> LGraph::edges() const
{
  std::vector<std::pair<LGraph::Vertex, LGraph::Edge>> edge_set;
  for (unsigned u = 0; u < countVertices(); u++)
  {
    for (auto v : m_adjacencyList[u])
    {
      if (u < v)
      {
        edge_set.emplace_back(u, v);
      }
    }
  }
  return edge_set;
}

unsigned LGraph::degree(Vertex v) const { return m_vertexDegrees[v]; }

std::vector<unsigned> LGraph::degree() const { return m_vertexDegrees; }

unsigned LGraph::label(Vertex v) const { return m_vertexLabels[v]; }

bool LGraph::isConnected() const
{
  throw std::runtime_error("Not implemented :(");
}

bool LGraph::isTree() const { throw std::runtime_error("Not implemented :("); }

bool LGraph::isBipartite() const
{
  throw std::runtime_error("Not implemented :(");
}

std::unique_ptr<TGraph<LGraph::Vertex, LGraph::Edge>> LGraph::complement() const
{
  throw std::runtime_error("Not implemented :(");
}

std::vector<std::unique_ptr<TGraph<LGraph::Vertex, LGraph::Edge>>>
LGraph::disjointSubgraphs(std::vector<std::vector<LGraph::Edge>> &subsets) const
{
  throw std::runtime_error("Not implemented :(");
}

std::unique_ptr<TGraph<LGraph::Vertex, LGraph::Edge>>
LGraph::quotient(std::vector<std::vector<LGraph::Edge>> &partition) const
{
  throw std::runtime_error("Not implemented :(");
}

std::vector<std::vector<LGraph::Edge>>
LGraph::modularPartition(std::vector<std::vector<LGraph::Edge>> &partition) const
{
  throw std::runtime_error("Not implemented :(");
}

std::vector<std::vector<LGraph::Edge>> LGraph::primeDecomposition() const
{
  throw std::runtime_error("Not implemented :(");
}

std::vector<std::vector<LGraph::Edge>> LGraph::adjacencyList() const
{
  return m_adjacencyList;
}

} // namespace graph
} // namespace banana
