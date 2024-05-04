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

using System;
using Gurobi;

class gc_funcnonlinear_cs {

   private static void printsol(GRBModel m, GRBVar x) {
     Console.WriteLine("x = " + x.X);
     Console.WriteLine("Obj = " + m.ObjVal);
   }

   static void Main() {
     try {

      // Create environment

      GRBEnv env = new GRBEnv();

      // Create a new model

      GRBModel m = new GRBModel(env);

      GRBVar x     = m.AddVar(-1.0, 4.0, 0.0, GRB.CONTINUOUS, "x");
      GRBVar twox  = m.AddVar(-2.0, 8.0, 0.0, GRB.CONTINUOUS, "twox");
      GRBVar sinx  = m.AddVar(-1.0, 1.0, 0.0, GRB.CONTINUOUS, "sinx");
      GRBVar cos2x = m.AddVar(-1.0, 1.0, 0.0, GRB.CONTINUOUS, "cos2x");
      GRBVar expx  = m.AddVar(0.0, GRB.INFINITY, 0.0, GRB.CONTINUOUS, "expx");

      // Set objective

      m.SetObjective(sinx + cos2x + 1, GRB.MINIMIZE);

      // Add linear constraints

      m.AddConstr(0.25*expx - x <= 0, "l1");
      m.AddConstr(2*x - twox == 0, "l2");

      // Add general function constraints
      // sinx = sin(x)
      GRBGenConstr gcf1 = m.AddGenConstrSin(x, sinx, "gcf1", "");
      // cos2x = cos(twox)
      GRBGenConstr gcf2 = m.AddGenConstrCos(twox, cos2x, "gcf2", "");
      // expx = exp(x)
      GRBGenConstr gcf3 = m.AddGenConstrExp(x, expx, "gcf3", "");

   // Approach 1) Set FuncNonlinear parameter

      m.Parameters.FuncNonlinear = 1;

      // Optimize the model and print solution

      m.Optimize();
      printsol(m, x);

      // Restore unsolved state and set parameter Funcnonlinear to its
      // default value
      m.Reset();
      m.Parameters.FuncNonlinear = 0;

   // Approach 2) Set FuncNonlinear attribute for every
   //             general function constraint

      gcf1.Set(GRB.IntAttr.FuncNonlinear, 1);
      gcf2.Set(GRB.IntAttr.FuncNonlinear, 1);
      gcf3.Set(GRB.IntAttr.FuncNonlinear, 1);

      // Optimize the model and print solution

      m.Optimize();
      printsol(m, x);

      // Dispose of model and environment

      m.Dispose();
      env.Dispose();
    } catch (GRBException e) {
      Console.WriteLine("Error code: " + e.ErrorCode + ". " + e.Message);
    }
  }
}
