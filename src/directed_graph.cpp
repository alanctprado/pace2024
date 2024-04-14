#include "../include/directed_graph.hpp"

#include <vector>
#include <queue>

Digraph::Digraph(int n) : adj(n), m(0) {}

void Digraph::add_edge(int u, int v) {
	// assumes the edge u -> v doesn't exist yet
    assert(u != v);
	this->adj[u].push_back(v);
    this->m++;
}

int Digraph::count_edges() const {
    return this->m;
}

int Digraph::count_vertices() const {
    return this->adj.size();
}

std::vector<int> Digraph::topological_order() const {
	int n = this->count_vertices();

	std::vector<int> indeg(n, 0);
	for (int i = 0; i < n; i++) {
		for (int j : adj[i]) {
			indeg[j]++;
		}
	}

	std::queue<int> queue;
	for (int i = 0; i < n; i++) {
		if (indeg[i] == 0)
			queue.push(i);
	}

	std::vector<int> order;
	while (!queue.empty()) {
		int u = queue.front();
		queue.pop();

		order.push_back(u);

		for (int v : adj[u]) {
			indeg[v] -= 1;
			if (indeg[v] == 0) {
				queue.push(v);
			}
		}
	}

	assert((int)order.size() == n);
	return order;
}
