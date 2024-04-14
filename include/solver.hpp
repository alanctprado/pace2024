#ifndef __SOLVER_HPP
#define __SOLVER_HPP

#include "graph.hpp"

// Finds the TWW contraction sequence for a connected graph
class Solver {
   public:
    Solver(Graph& G);
    void solve();
    void print_contraction();
    ContractionSequence get_contraction();

   private:
    ContractionSequence solve_tree();
    ContractionSequence solve_sat(int lb=0, int ub=-1);
    void tree_contractor(int root, int par);

    ContractionSequence cs;
    Graph& G;
};

#endif // ! __SOLVER_HPP
