# Copyright 2024, Gurobi Optimization, LLC
#
# This example considers the following nonconvex nonlinear problem
#
#  minimize   sin(x) + cos(2*x) + 1
#  subject to  0.25*exp(x) - x <= 0
#              -1 <= x <= 4
#
#  We show you two approaches to solve it as a nonlinear model:
#
#  1) Set the paramter FuncNonlinear = 1 to handle all general function
#     constraints as true nonlinear functions.
#
#  2) Set the attribute FuncNonlinear = 1 for each general function
#     constraint to handle these as true nonlinear functions.

library(gurobi)

printsol <- function(model, result) {
    print(sprintf('%s = %g',
                  model$varnames[1], result$x[1]))
    print(sprintf('Obj = %g',  + result$objval))
}

model <- list()

# Five variables, two linear constraints
model$varnames <- c('x', 'twox', 'sinx', 'cos2x', 'expx')
model$lb       <- c(-1, -2, -1, -1, 0)
model$ub       <- c(4, 8, 1, 1, Inf)
model$A        <- matrix(c(-1, 0, 0, 0, 0.25, 2, -1, 0, 0, 0), nrow=2, ncol=5, byrow=T)
model$rhs      <- c(0, 0)

# Objective
model$modelsense <- 'min'
model$obj        <- c(0, 0, 1, 1, 0)
model$objcon     <- 1

# Set sinx = sin(x)
model$genconsin           <- list()
model$genconsin[[1]]      <- list()
model$genconsin[[1]]$xvar <- 1L
model$genconsin[[1]]$yvar <- 3L
model$genconsin[[1]]$name <- 'gcf1'

# Set cos2x = cos(twox)
model$genconcos           <- list()
model$genconcos[[1]]      <- list()
model$genconcos[[1]]$xvar <- 2L
model$genconcos[[1]]$yvar <- 4L
model$genconcos[[1]]$name <- 'gcf2'

# Set expx = exp(x)
model$genconexp           <- list()
model$genconexp[[1]]      <- list()
model$genconexp[[1]]$xvar <- 1L
model$genconexp[[1]]$yvar <- 5L
model$genconexp[[1]]$name <- 'gcf3'

# First approach: Set Funcnonlinear parameter
params               <- list()
params$FuncNonlinear <- 1

# Solve and print solution
result = gurobi(model, params)
printsol(model, result)

# Second approach: Set FuncNonlinear attribute for every
#                  general function constraint

model$genconsin[[1]]$funcnonlinear <- 1
model$genconcos[[1]]$funcnonlinear <- 1
model$genconexp[[1]]$funcnonlinear <- 1

# Solve and print solution
result = gurobi(model)
printsol(model, result)

# Clear space
rm(model, result)
