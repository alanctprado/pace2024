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

import com.gurobi.gurobi.*;

public class GCFuncnonlinear {

   private static void printsol(GRBModel m, GRBVar x)
     throws GRBException {

     assert(m.get(GRB.IntAttr.Status) == GRB.OPTIMAL);
     System.out.println("x = " + x.get(GRB.DoubleAttr.X));
     System.out.println("Obj = " + m.get(GRB.DoubleAttr.ObjVal));
   }

   public static void main(String[] args) {
     try {

      // Create environment

      GRBEnv env = new GRBEnv();

      // Create a new model

      GRBModel m = new GRBModel(env);

      // Create variables

      GRBVar x     = m.addVar(-1.0, 4.0, 0.0, GRB.CONTINUOUS, "x");
      GRBVar twox  = m.addVar(-2.0, 8.0, 0.0, GRB.CONTINUOUS, "twox");
      GRBVar sinx  = m.addVar(-1.0, 1.0, 0.0, GRB.CONTINUOUS, "sinx");
      GRBVar cos2x = m.addVar(-1.0, 1.0, 0.0, GRB.CONTINUOUS, "cos2x");
      GRBVar expx  = m.addVar(0.0, GRB.INFINITY, 0.0, GRB.CONTINUOUS, "expx");

      // Set objective

      GRBLinExpr obj = new GRBLinExpr();
      obj.addTerm(1.0, sinx); obj.addTerm(1.0, cos2x); obj.addConstant(1.0);
      m.setObjective(obj, GRB.MINIMIZE);

      // Add linear constraints

      GRBLinExpr expr = new GRBLinExpr();
      expr.addTerm(0.25, expx); expr.addTerm(-1.0, x);
      m.addConstr(expr, GRB.LESS_EQUAL, 0.0, "l1");
      expr = new GRBLinExpr();
      expr.addTerm(2.0, x); expr.addTerm(-1.0, twox);
      m.addConstr(expr, GRB.EQUAL, 0.0, "l2");

      // Add general function constraints
      // sinx = sin(x)
      GRBGenConstr gcf1 = m.addGenConstrSin(x, sinx, "gcf1", null);
      // cos2x = cos(twox)
      GRBGenConstr gcf2 = m.addGenConstrCos(twox, cos2x, "gcf2", null);
      // expx = exp(x)
      GRBGenConstr gcf3 = m.addGenConstrExp(x, expx, "gcf3", null);

   // Approach 1) Set FuncNonlinear parameter

      m.set(GRB.IntParam.FuncNonlinear, 1);

      // Optimize the model and print solution

      m.optimize();
      printsol(m, x);

      // Restore unsolved state and set parameter FuncNonlinear to
      // its default value
      m.reset();
      m.set(GRB.IntParam.FuncNonlinear, 0);

   // Approach 2) Set FuncNonlinear attribute for every
   //             general function constraint

      gcf1.set(GRB.IntAttr.FuncNonlinear, 1);
      gcf2.set(GRB.IntAttr.FuncNonlinear, 1);
      gcf3.set(GRB.IntAttr.FuncNonlinear, 1);

      // Optimize the model and print solution

      m.optimize();
      printsol(m, x);

      // Dispose of model and environment

      m.dispose();
      env.dispose();

    } catch (GRBException e) {
      System.out.println("Error code: " + e.getErrorCode() + ". " +
          e.getMessage());
    }
  }
}
