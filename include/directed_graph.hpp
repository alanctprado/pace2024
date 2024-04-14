#ifndef __DIRECTED_GRAPH_HPP
#define __DIRECTED_GRAPH_HPP

#include <cassert>
#include <numeric>
#include <vector>
#include <utility>

class Digraph {
   public:
    Digraph(int n);
    void add_edge(int u, int v);
    int count_edges() const;
    int count_vertices() const;
	std::vector<int> topological_order() const;

   private:
    std::vector<std::vector<int>> adj;
    int m;
};

#endif // ! __DIRECTED_GRAPH_HPP
