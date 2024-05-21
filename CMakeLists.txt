cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Nome do projeto
project(pace2024)
# Versionamento
set (pace2024_VERSION_MAJOR 1)
set (pace2024_VERSION_MINOR 0)

# Headers
include_directories(src)
include_directories(lp_solve_5.5)

message("Building LP Solve")
execute_process(
  COMMAND sh compile_lpsolve.sh
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

set(SRC_FILES
  "${PROJECT_SOURCE_DIR}/src/barycenter_heuristic.cpp"
  "${PROJECT_SOURCE_DIR}/src/approximation_routine.cpp"
  "${PROJECT_SOURCE_DIR}/src/base_solver.cpp"
  "${PROJECT_SOURCE_DIR}/src/bipartite_graph.cpp"
  "${PROJECT_SOURCE_DIR}/src/environment.cpp"
  "${PROJECT_SOURCE_DIR}/src/graph.cpp"
  "${PROJECT_SOURCE_DIR}/src/ip_solver.cpp"
  "${PROJECT_SOURCE_DIR}/src/options.cpp"
  "${PROJECT_SOURCE_DIR}/src/oracle.cpp"
  "${PROJECT_SOURCE_DIR}/src/crossing_matrix.cpp"
  "${PROJECT_SOURCE_DIR}/src/wlgraph.cpp"
  "${PROJECT_SOURCE_DIR}/src/lgraph.cpp"
)

## Objetos comuns a todos os targets
add_library(common OBJECT ${SRC_FILES})

## Configuração para compilação do executável final
set(CMAKE_CXX_FLAGS "-Ofast")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
add_executable(pace $<TARGET_OBJECTS:common> "${PROJECT_SOURCE_DIR}/src/main.cpp")
target_link_libraries(pace "${PROJECT_SOURCE_DIR}/lp_solve_5.5/lpsolve55/bin/ux64/liblpsolve55.a" dl)

add_custom_target(run_tiny
  COMMAND sh run_set.sh tiny-set "$(ARGS)"
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_custom_target(run_medium
  COMMAND sh run_set.sh medium-set "$(ARGS)"
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
