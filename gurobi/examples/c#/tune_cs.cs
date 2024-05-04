/* Copyright 2024, Gurobi Optimization, LLC */

/* This example reads a model from a file and tunes it.
   It then writes the best parameter settings to a file
   and solves the model using these parameters. */

using System;
using Gurobi;

class tune_cs
{
  static void Main(string[] args)
  {
    if (args.Length < 1) {
      Console.Out.WriteLine("Usage: tune_cs filename");
      return;
    }

    try {
      GRBEnv env = new GRBEnv();

      // Read model from file
      GRBModel model = new GRBModel(env, args[0]);

      // Set the TuneResults parameter to 2
      //
      // The first parameter setting is the result for the first solved
      // setting. The second entry the parameter setting of the best
      // parameter setting.
      model.Parameters.TuneResults = 2;

      // Tune the model
      model.Tune();

      // Get the number of tuning results
      int resultcount = model.TuneResultCount;

      if (resultcount >= 2) {

        // Load the tuned parameters into the model's environment
        //
        // Note, the first parameter setting is associated to the first
        // solved setting and the second parameter setting to best tune
        // result.
        model.GetTuneResult(1);

        // Write the tuned parameters to a file
        model.Write("tune.prm");

        // Solve the model using the tuned parameters
        model.Optimize();
      }

      // Dispose of model and environment
      model.Dispose();
      env.Dispose();

    } catch (GRBException e) {
      Console.WriteLine("Error code: " + e.ErrorCode + ". " + e.Message);
    }
  }
}
