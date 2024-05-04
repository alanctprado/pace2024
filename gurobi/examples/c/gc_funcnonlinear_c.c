/* Copyright 2024, Gurobi Optimization, LLC

This example considers the following nonconvex nonlinear problem

  minimize   sin(x) + cos(2*x) + 1
  subject to  0.25*exp(x) - x <= 0
              -1 <= x <= 4

  We show you two approaches to solve it as a nonlinear model:

  1) Set the paramter FuncNonlinear = 1 to handle all general function
     constraints as true nonlinear functions.

  2) Set the attribute FuncNonlinear = 1 for each general function
     constraint to handle these as true nonlinear functions.

*/

#include <stdlib.h>
#include <stdio.h>
#include "gurobi_c.h"

static int
printsol(GRBmodel *m)
{
  double x[1];
  double vio;
  int    error = 0;

  error = GRBgetdblattrarray(m, "X", 0, 1, x);
  if (error) goto QUIT;

  printf("x = %g", x[0]);

QUIT:

  return error;
}

int
main(int   argc,
     char *argv[])
{
  GRBenv   *env     = NULL;
  GRBmodel *model   = NULL;
  int       error   = 0;
  int       attrs[] = {1, 1, 1};
  int       ind[2];
  double    val[2];

  /* Create environment */

  error = GRBloadenv(&env, NULL);
  if (error) goto QUIT;

  /* Create a new model */

  error = GRBnewmodel(env, &model, NULL, 0, NULL, NULL, NULL, NULL, NULL);
  if (error) goto QUIT;

  /* Add variables */

  error = GRBaddvar(model, 0, NULL, NULL, 0.0, -1.0, 4.0, GRB_CONTINUOUS, "x");
  if (error) goto QUIT;
  error = GRBaddvar(model, 0, NULL, NULL, 0.0, -2.0, 8.0, GRB_CONTINUOUS, "twox");
  if (error) goto QUIT;
  error = GRBaddvar(model, 0, NULL, NULL, 0.0, -1.0, 1.0, GRB_CONTINUOUS, "sinx");
  if (error) goto QUIT;
  error = GRBaddvar(model, 0, NULL, NULL, 0.0, -1.0, 1.0, GRB_CONTINUOUS, "cos2x");
  if (error) goto QUIT;
  error = GRBaddvar(model, 0, NULL, NULL, 0.0, 0.0, GRB_INFINITY, GRB_CONTINUOUS, "expx");
  if (error) goto QUIT;

  /* Add constant term to objective */

  error = GRBsetdblattr(model, "ObjCon", 1.0);

  /* Add linear constraint: 0.25*expx - x <= 0 */
  ind[0] = 4; ind[1] = 0;
  val[0] = 0.25; val[1] = -1.0;

  error = GRBaddconstr(model, 2, ind, val, GRB_LESS_EQUAL, 0.0, "c1");
  if (error) goto QUIT;

  /* Add linear constraint: 2*x - twox = 0 */
  ind[0] = 0; ind[1] = 1;
  val[0] = 2; val[1] = -1.0;

  error = GRBaddconstr(model, 2, ind, val, GRB_EQUAL, 0.0, "c2");
  if (error) goto QUIT;

  /* Add general function constraint: sinx = sin(x) */
  error = GRBaddgenconstrSin(model, "gcf1", 0, 2, NULL);
  if (error) goto QUIT;

  /* Add general function constraint: cos2x = cos(twox) */
  error = GRBaddgenconstrCos(model, "gcf2", 1, 3, NULL);
  if (error) goto QUIT;

  /* Add general function constraint: expx = exp(x) */
  error = GRBaddgenconstrExp(model, "gcf3", 0, 4, NULL);
  if (error) goto QUIT;

/* Approach 1) Set FuncNonlinear parameter */

  error = GRBsetintparam(GRBgetenv(model), "FuncNonlinear", 1);
  if (error) goto QUIT;

  /* Optimize the model and print solution */

  error = GRBoptimize(model);
  if (error) goto QUIT;

  error = printsol(model);
  if (error) goto QUIT;

  /* Restore unsolved state */
  error = GRBresetmodel(model);
  if (error) goto QUIT;

  /* Set FuncNonlinear parater back to its default value */
  error = GRBsetintparam(GRBgetenv(model), "FuncNonlinear", 0);
  if (error) goto QUIT;

/* Approach 2) Set FuncNonlinear attribute for every
               general function constraint */

  error = GRBsetintattrarray(model, "FuncNonlinear", 0, 3, attrs);
  if (error) goto QUIT;

  /* Optimize the model and print solution */

  error = GRBoptimize(model);
  if (error) goto QUIT;

  error = printsol(model);
  if (error) goto QUIT;

QUIT:

  if (error) {
    printf("ERROR: %s\n", GRBgeterrormsg(env));
    exit(1);
  }

  /* Free model */

  GRBfreemodel(model);

  /* Free environment */

  GRBfreeenv(env);

  return 0;
}
