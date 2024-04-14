/******************************************************************************
 * Top contributors (to current version):
 *   Alan Prado, Kaio Vieira, Laila Melo, Guilherme Gomes
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

#include "graph.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <numeric>

Graph::Graph(int n) : TGraph<int>()
{
  m_adjacencyMatrix.assign(n, std::vector<bool>(n, false));
  m_vertexDegrees.assign(n, 0);
  std::iota(m_vertexLabels.begin(), m_vertexLabels.end(), 0);
  m_edgeCount = 0;
}

Graph::Graph(const Graph &H)
{
  m_adjacencyMatrix = H.m_adjacencyMatrix;
  m_vertexDegrees = H.m_vertexDegrees;
  m_vertexLabels = H.m_vertexLabels;
  m_edgeCount = H.m_edgeCount;
}

void Graph::addEdge(int u, int v)
{
  assert(u != v);
  if (m_adjacencyMatrix[u][v]) return;
  m_adjacencyMatrix[u][v] = m_adjacencyMatrix[v][u] = true;
  m_vertexDegrees[u]++, m_vertexDegrees[v]++;
  m_edgeCount++;
}

void Graph::removeEdge(int u, int v)
{
  assert(u != v);
  if (!m_adjacencyMatrix[u][v]) return;
  m_adjacencyMatrix[u][v] = m_adjacencyMatrix[v][u] = false;
  m_vertexDegrees[u]--, m_vertexDegrees[v]--;
  m_edgeCount--;
}

bool Graph::hasEdge(int u, int v) const
{
  assert(u != v);
  return m_adjacencyMatrix[u][v];
}

unsigned Graph::countEdges() const
{
  return m_edgeCount;
}

unsigned Graph::countVertices() const
{
  return (unsigned) m_adjacencyMatrix.size();
}

std::vector<int> Graph::neighborhood(int u) const
{
  std::vector<int> neighbors;
  for (unsigned i = 0; i < countVertices(); i++)
  {
    if (m_adjacencyMatrix[u][i]) { neighbors.push_back(i); }
  }
  return neighbors;
}

std::vector<int> Graph::symmetricDifference(int u, int v) const
{
  std::vector<int> symmetric_difference;
  for (unsigned i = 0; i < countVertices(); i++)
  {
    if (i != u && i != v && m_adjacencyMatrix[u][i] != m_adjacencyMatrix[v][i])
    {
      symmetric_difference.push_back(i);
    }
  }
  return symmetric_difference;
}

std::vector<Graph::Edge> Graph::edges() const
{
  std::vector<Edge> edge_set;
  for (unsigned i = 0; i < countVertices(); i++)
  {
    for (int j = i + 1; j < countVertices(); j++)
    {
      if (m_adjacencyMatrix[i][j]) { edge_set.emplace_back(i, j); }
    }
  }
  return edge_set;
}

unsigned Graph::degree(int v) const
{
  return m_vertexDegrees[v];
}

std::vector<unsigned> Graph::degree() const
{
  return m_vertexDegrees;
}

unsigned Graph::label(int v) const
{
  return m_vertexLabels[v];
}

bool Graph::isConnected() const
{
  std::vector<bool> visited(countVertices(), false);
  std::vector<int> stack;

  stack.push_back(0);
  while (!stack.empty())
  {
    int u = stack.back();
    stack.pop_back();
    if (!visited[u])
    {
      visited[u] = true;
      for (int v : neighborhood(u)) 
      {
        if (!visited[v]) { stack.push_back(v); }
      }
    }
  }

  for (bool b : visited) { if (!b) { return false; } }
  return true;
}

bool Graph::isBipartite() const
{
  int n = countVertices();
  std::vector<int> color(n, -1);
  std::vector<int> stack;
  stack.push_back(0);
  color[0] = 0;

  while (!stack.empty())
  {
    int u = stack.back();
    stack.pop_back();
    for (int v : neighborhood(u))
    {
      if (color[v] == -1)
      {
        color[v] = !color[u];
        stack.push_back(v);
      }
      else if (color[v] == color[u]) { return false; }
    }
  }
  return true;
}

std::unique_ptr<TGraph<int>> Graph::complement() const
{
  Graph H(*this);
  for (unsigned i = 0; i < countVertices(); i++)
  {
    for (unsigned j = 0; j < countVertices(); j++)
    {
      if (i != j) { H.m_adjacencyMatrix[i][j] = !H.m_adjacencyMatrix[i][j]; }
    }
  }
  return std::make_unique<Graph>(H);
}

std::vector<std::unique_ptr<TGraph<int>>>
Graph::disjointSubgraphs(std::vector<std::vector<int>>& subsets) const
{
  std::vector<int> new_label(countVertices());
  std::vector<int> subset_id(countVertices());

  for (unsigned i = 0; i < (unsigned) subsets.size(); i++)
  {
    std::vector<int> subset_vertices = subsets[i];
    for (unsigned j = 0; j < (unsigned) subset_vertices.size(); j++)
    {
      new_label[subset_vertices[j]] = j;
      subset_id[subset_vertices[j]] = i;
    }
  }
  
  std::vector<std::unique_ptr<TGraph>> subgraphs;
  for (std::vector<int> subset_vertices : subsets)
  {
    Graph subgraph(subset_vertices.size());
    for (int u : subset_vertices)
    {
      subgraph.m_vertexLabels[new_label[u]] = m_vertexLabels[u];
      for (int v : neighborhood(u)) if (subset_id[v] == subset_id[u])
      {
        subgraph.addEdge(new_label[u], new_label[v]);
      }
    }
    subgraphs.push_back(std::make_unique<Graph> (subgraph));
  }
  return subgraphs;
}

std::unique_ptr<TGraph<int>>
Graph::quotient(std::vector<std::vector<int>>& partition) const
{
  Graph quotient_graph((int) partition.size());
  for (unsigned i = 0; i < (unsigned) partition.size(); i++)
  {
    int a = partition[i][0];
    for (unsigned j = i + 1; j < (unsigned) partition.size(); j++)
    {
      int b = partition[j][0];
      if (hasEdge(a, b)) quotient_graph.addEdge(i, j);
    }
  }
  return std::make_unique<Graph> (quotient_graph);
}

std::vector<std::vector<int>>
Graph::modularPartition(std::vector<std::vector<int>>& partition) const {
  const Graph& G = *this;
  std::vector<std::vector<int>> Q, K, L;
  Q = partition;

  for (std::vector<int> Z : partition)
  {
    if (K.empty())
    {
      K.push_back(Z);
      continue;
    }
    if (K[0].size() < Z.size()) { swap(K[0], Z); }
    L.push_back(Z);
  }

  while (L.size() or K.size())
  {
    std::vector<int> S;
    if (L.size())
    {
      S = L.back();
      L.pop_back();
    }
    else
    {
      S.push_back(K[0][0]);
      K.erase(K.begin());
    }

    for (int x : S)
    {
      std::vector<int> neig = G.neighborhood(x);
      std::sort(neig.begin(), neig.end());
      int q_size = (int) Q.size();
      for (int y_id = 0; y_id < q_size; y_id++)
      {
        std::vector<int> &Y = Q[y_id];
        bool is_x = false;
        for (int y : Y) { if (y == x) { is_x = true; } }

        if (is_x) { continue; }

        std::vector<int> in, out;
        for (int y : Y)
        {
          if (std::binary_search(neig.begin(), neig.end(), y))
          {
            in.push_back(y);
          }
          else { out.push_back(y); }
        }
        
        if (in.empty() or out.empty()) { continue; }

        std::vector<int> Y_min = in, Y_max = out;
        if (Y_min.size() > Y_max.size()) { std::swap(Y_min, Y_max); }
        
        int idx_l = -1, idx_k = -1;
        for (int i = 0; i < int(L.size()); i++)
        {
          if (Y[0] == L[i][0]) { idx_l = i; }
        }
        for (int i = 0; i < int(K.size()); i++)
        {
          if (Y[0] == K[i][0]) { idx_k = i; }
        }
        if (idx_l != -1)
        {
          L[idx_l] = Y_min;
          L.push_back(Y_max);
        }
        else
        {
          L.push_back(Y_min);
          if (idx_k != -1) { K[idx_k] = Y_max; }
          else K.push_back(Y_max);
        }
        
        Y = Y_max;
        Q.push_back(Y_min);
      }
    }
  }
  return Q;
}

std::vector<std::vector<int>> Graph::primeDecomposition() const
{
  int n = m_adjacencyMatrix.size();
  std::vector<std::vector<int>> ret;
  std::unique_ptr<TGraph> G_cur = std::make_unique<Graph>(n);
  std::unique_ptr<TGraph> G_new = std::make_unique<Graph>(n);

  for (int x = 0; x < n; x++)
  {
    if (x*n >= 2e5) { break; }
    std::vector<std::vector<int>> partition;
    partition.push_back({x});
    std::vector<int> in, out;
    for (int y = 0; y < n; y++) if (y != x)
    {
      if (m_adjacencyMatrix[x][y]) { in.push_back(y); }
      else out.push_back(y);
    }
    partition.push_back(in);
    partition.push_back(out);
    std::vector<std::vector<int>> modular = modularPartition(partition);
    if (ret.empty())
    {
      ret = modular;
      G_cur = quotient(modular);
    }
    else
    {
      G_new = this->quotient(modular);
      if (G_new->countEdges() * G_cur->countVertices() 
        < G_cur->countEdges() * G_new->countVertices())
      {
        ret = modular;
        G_cur = std::move(G_new);
      }
    }
  }

  for (std::vector<int> mod : ret)
  {
    std::vector<int> cnt(n);
    for (int i : mod) { cnt[i] = -1; }
    for (int i : mod)
    {
      for (int j : this->neighborhood(i))
      {
        if (cnt[j] != -1) { cnt[j]++; }
      }
    }
    for (int i = 0; i < n; i++)
    {
      assert(cnt[i] <= 0 or cnt[i] == int(mod.size()));
    }
  }
  return ret;
}
