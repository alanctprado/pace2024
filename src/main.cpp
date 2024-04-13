#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "../include/graph.hpp"

void read_next_line(std::istringstream& in)
{
	std::string line;
	while (std::getline(std::cin, line)) {
		if (line[0] == 'c') continue;
		in = std::istringstream(line);
		return;
	}
	assert(false);
}

BipartiteGraph* read_graph()
{
	std::istringstream in;
	read_next_line(in);

	std::string s;
	int n0, n1, m, a, b;
	in >> s >> s >> n0 >> n1 >> m;

	BipartiteGraph* g = new BipartiteGraph(n0, n1);
	for (int i = 0; i < m; i++)
  {
		read_next_line(in);
		in >> a >> b;
		g->add_edge(a - 1, b - 1);
	}

	return g;
}

int main(int argc, char* argv[])
{
	BipartiteGraph* g = read_graph();

  std::vector<std::vector<int>> cm = g->build_crossing_matrix();

  int n = g->count_vertices_b();
  int offset = g->count_vertices_a();

  std::cout << offset << std::endl;

	exit(0);
}
