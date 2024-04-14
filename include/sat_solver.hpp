#ifndef __SAT_SOLVER_HPP
#define __SAT_SOLVER_HPP

#include "../cadical/src/cadical.hpp"

#include <vector>

class SatSolver {
   public:
    SatSolver();
    SatSolver(const SatSolver& s);
    int new_variable();
    void add_clause(std::vector<int> clause);
    void add_auxiliary_constraints(std::vector<int> variables); // will call only once per object
    void add_cardinality_constraints(int ub, int lb = 0);
    int solve();
	int val(int);
    
   private:
    int index;
    int t;
    int f;
    CaDiCaL::Solver solver;
    std::vector<std::vector<int>> constraints;
};

#endif // ! __SAT_SOLVER_HPP
