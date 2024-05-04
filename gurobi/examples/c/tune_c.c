/* Copyright 2024, Gurobi Optimization, LLC */

/* This example reads a model from a file and tunes it.
   It then writes the best parameter settings to a file
   and solves the model using these parameters. */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "gurobi_c.h"

int
main(int   argc,
     char *argv[])
{
  GRBenv   *env   = NULL;
  GRBmodel *model = NULL;
  int       tuneresultcount;
  int       error = 0;

  if (argc < 2) {
    fprintf(stderr, "Usage: tune_c filename\n");
    exit(1);
  }

  /* Create environment */

  error = GRBloadenv(&env, "tune_c.log");
  if (error) goto QUIT;

  /* Read model from file */

  error = GRBreadmodel(env, argv[1], &model);
  if (error) goto QUIT;

  /* Set the TuneResults parameter to 2
   *
   * The first parameter setting is the result for the first solved
   * setting. The second entry the parameter setting of the best parameter
   * setting.
   */
  error = GRBsetintparam(GRBgetenv(model), GRB_INT_PAR_TUNERESULTS, 2);
  if (error) goto QUIT;

  /* Tune the model */

  error = GRBtunemodel(model);
  if (error) goto QUIT;

  /* Get the number of tuning results */

  error = GRBgetintattr(model, GRB_INT_ATTR_TUNE_RESULTCOUNT, &tuneresultcount);
  if (error) goto QUIT;

  if (tuneresultcount >= 2) {

    /* Load the best tuned parameters into the model's environment
     *
     * Note, the first parameter setting is associated to the first solved
     * setting and the second parameter setting to best tune result.
     */
    error = GRBgettuneresult(model, 1);
    if (error) goto QUIT;

    /* Write tuned parameters to a file */

    error = GRBwrite(model, "tune.prm");
    if (error) goto QUIT;

    /* Solve the model using the tuned parameters */

    error = GRBoptimize(model);
    if (error) goto QUIT;
  }

QUIT:

  /* Error reporting */

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
