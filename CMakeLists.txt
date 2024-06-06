cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Nome do projeto
project(pace2024)
# Versionamento
set (pace2024_VERSION_MAJOR 1)
set (pace2024_VERSION_MINOR 0)

option(USE_OR_TOOLS "Use OR-Tools interface" OFF)
option(USE_GUROBI "Use Gurobi IP Solver" OFF)
option(USE_LPSOLVE "Use LPSolve IP Solver" ON)

# Headers
include(ExternalProject)
include_directories(src)

if(USE_LPSOLVE)
  add_definitions(-DUSE_LPSOLVE)
  include_directories(lp_solve_5.5)
  message("Building LP Solve")
  execute_process(
    COMMAND sh compile_lpsolve.sh
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )
endif()

if(USE_GUROBI)
  include_directories(gurobi/include)
endif()

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/deps")

if(USE_GUROBI)
  add_definitions(-DUSE_GUROBI)
  message("Building Gurobi")
  execute_process(
    COMMAND sh compile_gurobi.sh
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )
endif()

if(USE_OR_TOOLS)
  add_definitions(-DUSE_OR_TOOLS)
  
  # Specify the path to the or-tools library and include directories
  set(ORTOOLS_ROOT /or-tools)
  include_directories(${ORTOOLS_ROOT}/include)
  link_directories(${ORTOOLS_ROOT}/lib)
endif()

set(SRC_FILES
  "${PROJECT_SOURCE_DIR}/src/barycenter_heuristic.cpp"
  "${PROJECT_SOURCE_DIR}/src/median_heuristic.cpp"
  "${PROJECT_SOURCE_DIR}/src/approximation_routine.cpp"
  "${PROJECT_SOURCE_DIR}/src/base_solver.cpp"
  "${PROJECT_SOURCE_DIR}/src/bipartite_graph.cpp"
  "${PROJECT_SOURCE_DIR}/src/environment.cpp"
  "${PROJECT_SOURCE_DIR}/src/graph.cpp"
  "${PROJECT_SOURCE_DIR}/src/ip_solver_lpsolve.cpp"
  "${PROJECT_SOURCE_DIR}/src/ip_solver_gurobi.cpp"
  "${PROJECT_SOURCE_DIR}/src/ip_solver_or.cpp"
  "${PROJECT_SOURCE_DIR}/src/options.cpp"
  "${PROJECT_SOURCE_DIR}/src/preprocessing.cpp"
  "${PROJECT_SOURCE_DIR}/src/meta_solver.cpp"
  "${PROJECT_SOURCE_DIR}/src/oracle.cpp"
  "${PROJECT_SOURCE_DIR}/src/wlgraph.cpp"
  "${PROJECT_SOURCE_DIR}/src/lgraph.cpp"
  "${PROJECT_SOURCE_DIR}/src/caterpillar_recognizer.cpp"
  "${PROJECT_SOURCE_DIR}/src/subdivided_wheel_recognizer.cpp"
  "${PROJECT_SOURCE_DIR}/src/starforest_recognizer.cpp"
  "${PROJECT_SOURCE_DIR}/src/outerplanar_recognizer.cpp"
  "${PROJECT_SOURCE_DIR}/src/lobster_recognizer.cpp"
  "${PROJECT_SOURCE_DIR}/src/crossing_matrix.cpp")

## Objetos comuns a todos os targets
add_library(common OBJECT ${SRC_FILES})

## Configuração para compilação do executável final
set(CMAKE_CXX_FLAGS "-Ofast")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
add_executable(pace $<TARGET_OBJECTS:common> "${PROJECT_SOURCE_DIR}/src/main.cpp")

if(USE_OR_TOOLS)
  target_link_libraries(pace ortools)
endif()

if(USE_GUROBI)
  target_link_libraries(pace "${PROJECT_SOURCE_DIR}/gurobi/src/build/libgurobi_c++.a" dl)
  target_link_libraries(pace "${PROJECT_SOURCE_DIR}/gurobi/lib/libgurobi110.so" dl)
endif()

if(USE_LPSOLVE)
  target_link_libraries(pace "${PROJECT_SOURCE_DIR}/lp_solve_5.5/lpsolve55/bin/ux64/liblpsolve55.a" dl)
endif()

add_custom_target(run_tiny
  COMMAND sh run_set.sh tiny-set "$(ARGS)"
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_custom_target(run_medium
  COMMAND sh run_set.sh medium-set "$(ARGS)"
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
