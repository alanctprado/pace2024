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
 * Basic graph class with weighted edges and adjacency list representation.
 */

#include "wlgraph.h"

#include <algorithm>
#include <cassert>
#include <numeric>

namespace banana {
namespace graph {

WLGraph::WLGraph(int n) : TGraph<Vertex, Edge>()
{
  m_adjacencyList.assign(n, std::vector<WLGraph::Edge>());
  m_vertexDegrees.assign(n, 0);
  std::iota(m_vertexLabels.begin(), m_vertexLabels.end(), 0);
  m_edgeCount = 0;
}

WLGraph::WLGraph(const WLGraph &H)
{
  m_adjacencyList = H.m_adjacencyList;
  m_vertexDegrees = H.m_vertexDegrees;
  m_vertexLabels = H.m_vertexLabels;
  m_edgeCount = H.m_edgeCount;
}

void WLGraph::addEdge(Vertex v, Edge e)
{
  assert(v != e.to);
  m_adjacencyList[v].push_back(e);
  m_adjacencyList[e.to].push_back(WLGraph::Edge(v, e.weight));
  m_vertexDegrees[v]++, m_vertexDegrees[e.to]++;
  m_edgeCount++;
}

void WLGraph::removeEdge(Vertex v, Edge e)
{
  assert(v != e.to);
  auto it1 = std::find(begin(m_adjacencyList[v]), end(m_adjacencyList[e.to]), e);

  if (it1 != end(m_adjacencyList[v]))
  {
    m_adjacencyList[v].erase(it1);
  }

  auto it2 = std::find(begin(m_adjacencyList[e.to]), end(m_adjacencyList[e.to]), WLGraph::Edge(v, e.weight));

  if (it2 != end(m_adjacencyList[e.to]))
  {
    m_adjacencyList[e.to].erase(it2);
  }

  m_vertexDegrees[v]--, m_vertexDegrees[e.to]--;
  m_edgeCount--;
}

bool WLGraph::hasEdge(Vertex v, Edge e) const
{
  assert(v != e.to);
  return std::find(begin(m_adjacencyList[v]), end(m_adjacencyList[v]), e) != end(m_adjacencyList[v]);
}

unsigned WLGraph::countEdges() const { return m_edgeCount; }

unsigned WLGraph::countVertices() const
{
  return (unsigned)m_adjacencyList.size();
}

std::vector<WLGraph::Edge> WLGraph::neighborhood(Vertex v) const
{
  return m_adjacencyList[v];
}
std::vector<WLGraph::Edge> WLGraph::symmetricDifference(Vertex v, Vertex u) const
{
  throw std::runtime_error("Not implemented :(");
}

std::vector<std::pair<WLGraph::Vertex, WLGraph::Edge>> WLGraph::edges() const
{
  std::vector<std::pair<WLGraph::Vertex, WLGraph::Edge>> edge_set;
  for (unsigned u = 0; u < countVertices(); u++)
  {
    for (auto e : m_adjacencyList[u])
    {
      if (u < e.to)
      {
        edge_set.emplace_back(u, e);
      }
    }
  }
  return edge_set;
}

unsigned WLGraph::degree(Vertex v) const { return m_vertexDegrees[v]; }

std::vector<unsigned> WLGraph::degree() const { return m_vertexDegrees; }

unsigned WLGraph::label(Vertex v) const { return m_vertexLabels[v]; }

bool WLGraph::isConnected() const
{
  throw std::runtime_error("Not implemented :(");
}

bool WLGraph::isTree() const { throw std::runtime_error("Not implemented :("); }

bool WLGraph::isBipartite() const
{
  throw std::runtime_error("Not implemented :(");
}

std::unique_ptr<TGraph<WLGraph::Vertex, WLGraph::Edge>> WLGraph::complement() const
{
  throw std::runtime_error("Not implemented :(");
}

std::vector<std::unique_ptr<TGraph<WLGraph::Vertex, WLGraph::Edge>>>
WLGraph::disjointSubgraphs(std::vector<std::vector<Edge>> &subsets) const
{
  throw std::runtime_error("Not implemented :(");
}

std::unique_ptr<TGraph<WLGraph::Vertex, WLGraph::Edge>>
WLGraph::quotient(std::vector<std::vector<Edge>> &partition) const
{
  throw std::runtime_error("Not implemented :(");
}

std::vector<std::vector<WLGraph::Edge>>
WLGraph::modularPartition(std::vector<std::vector<Edge>> &partition) const
{
  throw std::runtime_error("Not implemented :(");
}

std::vector<std::vector<WLGraph::Edge>> WLGraph::primeDecomposition() const
{
  throw std::runtime_error("Not implemented :(");
}

std::vector<std::vector<WLGraph::Edge>> WLGraph::adjacencyList() const
{
  return m_adjacencyList;
}

} // namespace graph
} // namespace banana
