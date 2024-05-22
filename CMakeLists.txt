cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Nome do projeto
project(pace2024)
# Versionamento
set (pace2024_VERSION_MAJOR 1)
set (pace2024_VERSION_MINOR 0)

# Headers
include(ExternalProject)
include_directories(src)
include_directories(lp_solve_5.5)
include_directories(gurobi/include)

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/deps")

message("Building LP Solve")
execute_process(
  COMMAND sh compile_lpsolve.sh
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

message("Building Gurobi")
execute_process(
  COMMAND sh compile_gurobi.sh
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

# include(FetchContent)
# FetchContent_Declare(
#   or-tools
#   GIT_REPOSITORY https://github.com/google/or-tools.git
#   GIT_TAG        v9.10)
# set(BUILD_DEPS ON)
# set(BUILD_DOC OFF)
# set(BUILD_SAMPLES OFF)
# set(BUILD_EXAMPLES OFF)
# FetchContent_MakeAvailable(or-tools)
# Include the ExternalProject module

# Define the OR-Tools version and repository
set(ORTOOLS_VERSION "9.10")
set(ORTOOLS_REPO "https://github.com/google/or-tools.git")

# Define the installation prefix for OR-Tools
set(ORTOOLS_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/deps/or-tools")

# Add the external project for OR-Tools
ExternalProject_Add(
    or-tools
    PREFIX ${ORTOOLS_INSTALL_PREFIX}
    GIT_REPOSITORY ${ORTOOLS_REPO}
    GIT_TAG v${ORTOOLS_VERSION}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${ORTOOLS_INSTALL_PREFIX} -DBUILD_DEPS=ON
    BUILD_IN_SOURCE 0
    UPDATE_DISCONNECTED 1
    INSTALL_DIR ${EXTERNAL_INSTALL_PREFIX}
)

# Add the include directories
include_directories(${ORTOOLS_INSTALL_PREFIX}/include)

# Find the OR-Tools library
find_library(ORTOOLS_LIB ortools PATHS ${ORTOOLS_INSTALL_PREFIX}/lib)

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
  "${PROJECT_SOURCE_DIR}/src/wlgraph.cpp"
  "${PROJECT_SOURCE_DIR}/src/lgraph.cpp"
  "${PROJECT_SOURCE_DIR}/src/caterpillar_recognizer.cpp"
  "${PROJECT_SOURCE_DIR}/src/subdivided_wheel_recognizer.cpp"
  "${PROJECT_SOURCE_DIR}/src/starforest_recognizer.cpp"
  "${PROJECT_SOURCE_DIR}/src/outerplanar_recognizer.cpp"
  "${PROJECT_SOURCE_DIR}/src/lobster_recognizer.cpp")

## Objetos comuns a todos os targets
add_library(common OBJECT ${SRC_FILES})

## Configuração para compilação do executável final
set(CMAKE_CXX_FLAGS "-Ofast")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
add_executable(pace $<TARGET_OBJECTS:common> "${PROJECT_SOURCE_DIR}/src/main.cpp")
target_link_libraries(pace "${PROJECT_SOURCE_DIR}/lp_solve_5.5/lpsolve55/bin/ux64/liblpsolve55.a" dl)
target_link_libraries(pace "${PROJECT_SOURCE_DIR}/gurobi/src/build/libgurobi_c++.a" dl)
target_link_libraries(pace "${PROJECT_SOURCE_DIR}/gurobi/lib/libgurobi110.so" dl)
#target_link_libraries(pace "${PROJECT_SOURCE_DIR}/or-tools/ortools/linear_solver/linear_solver.h/gurobi/lib/libgurobi110.so" dl)
# target_link_libraries(pace ortools::ortools)

add_custom_target(run_tiny
  COMMAND make -C build
  COMMAND sh run_set.sh tiny-set "$(ARGS)"
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_custom_target(run_medium
  COMMAND sh run_set.sh medium-set "$(ARGS)"
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
