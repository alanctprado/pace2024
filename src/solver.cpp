#include "../include/directed_graph.hpp"
#include "../include/solver.hpp"
#include "../include/sat_solver.hpp"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <vector>
#include <set>
#include <map>
#include <iostream>

Solver::Solver(Graph& _G) : G(_G) {
    assert(G.is_connected());
}

ContractionSequence Solver::solve_tree() {
    int n = this->G.count_vertices(); assert(n > 1);

    std::vector<int> dist(n, -1);
    std::vector<int> stack;
    stack.push_back(0);
    dist[0] = 0;

    while (!stack.empty()) {
        int u = stack.back();
        stack.pop_back();
        for (auto v : this->G.neighborhood(u)) if (dist[v] == -1) {
            dist[v] = dist[u] + 1;
            stack.push_back(v);
        }
    }

    int root = std::distance(
            dist.begin(),
            std::max_element(dist.begin(), dist.end())
            );

    dist.assign(n, -1);
    std::vector<int> par(n, -1);
    stack.push_back(root);
    dist[root] = 0;

    while (!stack.empty()) {
        int u = stack.back();
        stack.pop_back();
        for (auto v : this->G.neighborhood(u)) if (dist[v] == -1) {
            par[v] = u;
            dist[v] = dist[u] + 1;
            stack.push_back(v);
        }
    }

    int end = std::distance(
            dist.begin(),
            std::max_element(dist.begin(), dist.end())
            );

    std::vector<int> diameter;
    while(par[end] != -1) {
        diameter.push_back(end);
        end = par[end];
    }
    diameter.push_back(end);

    std::vector<int> caterpillar(n, 0);
    for (auto u : diameter) {
        for (auto v : this->G.neighborhood(u)) {
            caterpillar[v] = 1;
        }
    }

    this->tree_contractor(root, -1);
    return cs;
}

void Solver::tree_contractor(int root, int par) {
    auto nb = this->G.neighborhood(root);

    if (par != -1)
        nb.erase(std::find(nb.begin(), nb.end(), par));
    if (nb.empty())
        return;

    this->tree_contractor(nb[0], root);
    for (int i = 1; i < (int) nb.size(); i++) {
        this->tree_contractor(nb[i], root);
        this->cs.emplace_back(nb[0], nb[i]);
    }

    this->cs.emplace_back(root, nb[0]);
}

void Solver::solve() {
    if (this->G.count_vertices() == 1) {
    	this->cs.clear();
	}
    else if (this->G.is_tree()) {
       	this->cs = this->solve_tree();     
    }
    else {
		auto [ub, seq] = G.greedy_upper_bound();
		int lb = std::max(1, G.greedy_lower_bound());
		if (lb == ub)
			this->cs = seq;
		else
			this->cs = this->solve_sat(lb, ub);
    }
}

void Solver::print_contraction() {
    for (auto [p, u] : this->cs)
        std::cout << G.label(p) + 1 << " " << G.label(u) + 1 << std::endl;
}

ContractionSequence Solver::get_contraction() {
    return this->cs;
}

ContractionSequence Solver::solve_sat(int lb, int ub) {
    int n = G.count_vertices();
    if (ub == -1) ub = n - 1;

    SatSolver solver;

    // Indexing o-variables
    std::map<std::pair<int, int>, int> o_variables;
    std::map<int, std::pair<int, int>> o_reverse_index;
    auto o = [&] (int i, int j) {
        assert(i != j);
        bool sign = false;
        if (i > j) {
            std::swap(i, j);
            sign = true;
        }
        assert(o_variables.count(std::pair(i, j)));
        return (sign ? -1 : 1) * o_variables[std::pair(i, j)];
    };
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            o_variables[std::pair(i, j)] = solver.new_variable();
			o_reverse_index[o_variables[std::pair(i, j)]] = std::pair(i, j);
        }
    }

    // Indexing p-variables
    std::map<std::pair<int, int>, int> p_variables;
    std::map<int, std::pair<int, int>> p_reverse_index;
    auto p = [&] (int i, int j) {
        assert(i != j);
        bool sign = false;
        if (i > j) {
            std::swap(i, j);
            sign = true;
        }
        assert(p_variables.count(std::pair(i, j)));
        return (sign ? -1 : 1) * p_variables[std::pair(i, j)];
    };
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            p_variables[std::pair(i, j)] = solver.new_variable();
			p_reverse_index[p_variables[std::pair(i, j)]] = std::pair(i, j);
        }
    }

    // Indexing a-variables
    std::map<std::pair<int, int>, int> a_variables;
    std::map<int, std::pair<int, int>> a_reverse_index;
    auto a = [&] (int i, int j) {
        assert(i != j);
        if (i > j) 
            std::swap(i, j);
        assert(a_variables.count(std::pair(i, j)));
        return a_variables[std::pair(i, j)];
    };
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            a_variables[std::pair(i, j)] = solver.new_variable();
			a_reverse_index[a_variables[std::pair(i, j)]] = std::pair(i, j);
        }
    }

    // Indexing r-variables
    std::map<std::tuple<int, int, int>, int> r_variables;
    std::map<int, std::tuple<int, int, int>> r_reverse_index;
    auto r = [&] (int i, int j, int k) {
        assert(j != k);
        if (j > k)
            std::swap(j, k);
        assert(r_variables.count(std::tuple(i, j, k)));
        return r_variables[std::tuple(i, j, k)];
    };
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = j + 1; k < n; k++) {
                r_variables[std::tuple(i, j, k)] = solver.new_variable();
				r_reverse_index[r_variables[std::tuple(i, j, k)]] = std::tuple(i, j, k);
            }
        }
    }

    // Creating model restrictions

    // restriction: transitivity of the order
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                if (i != j && i != k && j != k) {
                    std::vector<int> clause = {
                        -o(i, j), -o(j, k), o(i, k)
                    };
                    solver.add_clause(clause);
                }
            }
        }
    }

    // restriction: at least one parent per vertex
    for (int i = 0; i < n - 1; i++) {
        std::vector<int> clause;
        for (int j = i + 1; j < n; j++) {
            clause.push_back(p(i, j));
        }
        solver.add_clause(clause);
    }

    // restriction: at most one parent per vertex
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
			for (int k = j + 1; k < n; k++) {
				std::vector<int> clause = {
					-p(i, j), -p(i, k)
				};
				solver.add_clause(clause);
			}
        }
    }

    // restriction: the parent comes next in the order
    for (int i = 0; i < n; i++) {
		for (int j = i + 1; j < n; j++) {
			std::vector<int> clause = {
				-p(i, j), o(i, j)
			};
			solver.add_clause(clause);
		}
    }

    // restriction: bookkeeping of 'r' and 'a'
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            for (int k = 0; k < n; k++) {
                if (i != k && j != k) {
                    std::vector<int> clause = {
                        -o(i, j), -o(i, k), -r(i, j, k), a(j, k)
                    };
                    solver.add_clause(clause);
                }
            }
        }
    }

    // restriction: edges in the symmetric difference
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            for (int k : G.symmetric_difference(i, j)) {
                std::vector<int> clause = {
                    -p(i, j), -o(i, k), r(i, j, k)
                };
                solver.add_clause(clause);
            }
        }
    }

    // restriction: inheritance of edges
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            for (int k = 0; k < n; k++) {
                if (i != k && j != k) {
                    std::vector<int> clause = {
                        -p(i, j), -o(i, k), -a(i, k), r(i, j, k)
                    };
                    solver.add_clause(clause);
                }
            }
        }
    }

    // restriction: edges from H_i keep existing in H_{i+1}
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                for (int m = k + 1; m < n; m++) {
                    if (i != j && i != k && i != m && j != k && j != m) {
                        std::vector<int> clause = {
                            -o(i, j), -o(j, k), -o(j, m), -r(i, k, m), r(j, k, m)
                        };
                        solver.add_clause(clause);
                    }
                }
            }
        }
    }

	// cardinality constraints
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			std::vector<int> clause;
			for (int k = 0; k < n; k++) if (j != k) {
				clause.push_back(r(i, j, k));
			}
			solver.add_auxiliary_constraints(clause);
		}
	}

	auto get_sequence_from_solver = [&] (SatSolver& solver) {
		std::vector<int> parent(n);
		for (int i = 0; i < n; i++) {
			for (int j = i + 1; j < n; j++) {
				if (solver.val(p(i, j)) > 0)
					parent[i] = j;
			}
		}

		Digraph order_graph(n);
		for (int i = 0; i < n; i++) {
			for (int j = i + 1; j < n; j++) {
				if (solver.val(o(i, j)) > 0)
					order_graph.add_edge(i, j);
				else
					order_graph.add_edge(j, i);
			}
		}

		auto elimination_order = order_graph.topological_order();
		ContractionSequence seq;
		for (int i = 0; i < n - 1; i++) {
			int u = elimination_order[i];
			seq.emplace_back(parent[u], u);
		}

		return seq;
	};

	std::pair<int, ContractionSequence> best(ub + 1, {});

	while (lb < ub) {
		int mid = lb + (ub - lb) / 2;

		SatSolver copy = solver;
		copy.add_cardinality_constraints(mid, lb);

		if (copy.solve() == 10) {
			ub = mid;

			if (ub < best.first) {
				best = std::pair(ub, get_sequence_from_solver(copy));
			}
		} else
			lb = mid + 1;
	}

	ContractionSequence seq;

	if (best.first == lb) {
		seq = best.second;
	} else {
		solver.add_cardinality_constraints(lb, lb);
		assert(solver.solve() == 10);
		seq = get_sequence_from_solver(solver);
	}

	assert(G.width(seq) == lb);

    return seq;
}
