/* Copyright 2024, Gurobi Optimization, LLC */

/* This example reads a model from a file and tunes it.
   It then writes the best parameter settings to a file
   and solves the model using these parameters. */

#include "gurobi_c++.h"
#include <cmath>
using namespace std;

int
main(int   argc,
     char *argv[])
{
  if (argc < 2) {
    cout << "Usage: tune_c++ filename" << endl;
    return 1;
  }

  GRBEnv *env = 0;
  try {
    env = new GRBEnv();

    // Read model from file

    GRBModel model = GRBModel(*env, argv[1]);

    // Set the TuneResults parameter to 2
    //
    // The first parameter setting is the result for the first solved
    // setting. The second entry the parameter setting of the best
    // parameter setting.

    model.set(GRB_IntParam_TuneResults, 2);

    // Tune the model

    model.tune();

    // Get the number of tuning results

    int resultcount = model.get(GRB_IntAttr_TuneResultCount);

    if (resultcount >= 2) {

      // Load the tuned parameters into the model's environment
      //
      // Note, the first parameter setting is associated to the first solved
      // setting and the second parameter setting to best tune result.

      model.getTuneResult(1);

      // Write tuned parameters to a file

      model.write("tune.prm");

      // Solve the model using the tuned parameters

      model.optimize();
    }
  } catch(GRBException e) {
    cout << "Error code = " << e.getErrorCode() << endl;
    cout << e.getMessage() << endl;
  } catch (...) {
    cout << "Error during tuning" << endl;
  }

  delete env;
  return 0;
}
