function gc_funcnonlinear

% Copyright 2024, Gurobi Optimization, LLC
%
% This example considers the following nonconvex nonlinear problem
%
%  minimize   sin(x) + cos(2*x) + 1
%  subject to  0.25*exp(x) - x <= 0
%              -1 <= x <= 4
%
%  We show you two approaches to solve it as a nonlinear model:
%
%  1) Set the paramter FuncNonlinear = 1 to handle all general function
%     constraints as true nonlinear functions.
%
%  2) Set the attribute FuncNonlinear = 1 for each general function
%     constraint to handle these as true nonlinear functions.
%


% Five variables, two linear constraints
m.varnames = {'x', 'twox', 'sinx', 'cos2x', 'expx'};
m.lb = [-1 -2 -1 -1 0];
m.ub = [4 8 1 1 +inf];
m.A = sparse([-1 0 0 0 0.25; 2 -1 0 0 0]);
m.rhs = [0; 0];

% Objective
m.modelsense = 'min';
m.obj = [0; 0; 1; 1; 0];

% Add general function constraints
% sinx = sin(x)
m.genconsin.xvar = 1;
m.genconsin.yvar = 3;
m.genconsin.name = 'gcf1';

m.genconcos.xvar = 2;
m.genconcos.yvar = 4;
m.genconcos.name = 'gcf2';

m.genconexp.xvar = 1;
m.genconexp.yvar = 5;
m.genconexp.name = 'gcf3';

% First approach: Set FuncNonlinear parameter

params.FuncNonlinear = 1;

% Solve and print solution
result = gurobi(m, params);
printsol(result.objval, result.x(1));

% Second approach: Set FuncNonlinear attribute for every
%                  general function constraint

m.genconsin.funcnonlinear = 1
m.genconcos.funcnonlinear = 1
m.genconexp.funcnonlinear = 1

% Solve and print solution
result = gurobi(m);
printsol(result.objval, result.x(1));
end

function printsol(objval, x)
    fprintf('x = %g\n', x);
    fprintf('Obj = %g\n', objval);
end
