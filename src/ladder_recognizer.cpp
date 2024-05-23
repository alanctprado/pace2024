/******************************************************************************
 * Top contributors (to current version):
 *  Laila
 *
 * This file is part of Banana, a one-sided crossing minimization solver.
 *
 * Copyright (c) 2024 by the authors listed in the file AUTHORS in the
 * top-level source directory and their institutional affiliations. All rights
 * reserved. See the file LICENSE.md in the top-level source directory for
 * licensing information.
 * ****************************************************************************
 *
 * Ladder Recognizer
 */

#include "ladder_recognizer.h"

namespace banana {
namespace recognizer {

/*
    Basically it 
*/

LadderRecognizer::LadderRecognizer(graph::Graph &graph)
    : Recognizer(graph)
{}

bool LadderRecognizer::check()
{
    unsigned n = m_graph.countVertices();
    std::vector<unsigned> degree = m_graph.degree();

    for(int i=0; i<n; i++)
    {
      if(degree[i]!=3)
      {
        return false;
      }
    }

    int x = -1;
    std:: vector<int> first_nb = m_graph.neighborhood(0);
    for(auto u: m_graph.neighborhood(first_nb[0]))
      for(auto v: m_graph.neighborhood(u))
        if(v==first_nb[1]){
          x = u;
        }

    if(x==-1) return false;  

    std:: vector<int> vis(n); 
    vis[0] = true; vis[first_nb[0]] = true; vis[first_nb[1]] = true; vis[x]=true;

    int v1 = first_nb[2];
    int v2 = -1;
    for(auto u: m_graph.neighborhood(v1)){
      int cnt = 0;
      for(auto v: m_graph.neighborhood(u)) if(vis[v]) cnt++;
      if(cnt==1) v2=u;
    }


    if(v2==-1) return false;

    std::vector<std::pair<int, int>> stack;
    stack.push_back({v1, v2});
    while(!stack.empty())
    {
      auto [x, y] = stack.back(); stack.pop_back();
      vis[x] = true;
      vis[y] = true;

      int x1 = -1, x2=-1;
      for(auto u: m_graph.neighborhood(x)) if(!vis[u]) x1 = u;
      for(auto u: m_graph.neighborhood(y)) if(!vis[u]) x2 = u;

      if(x1==-1 and x2==-1) break;
      else if(x1!=-1 and x2!=-1) {
        if(m_graph.hasEdge(x1, x2)) stack.push_back({x1, x2});
        else return false;
      }
      else return false;
    }

    int ok = 1;
    for(int i=0; i<n; i++) if(!vis[i]) ok = false;
    if(!ok) return false;
    return true;
    
}

} // namespace recognizer
} // namespace banana
