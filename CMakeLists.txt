cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Nome do projeto
project(pace2023)
# Versionamento
set (pace2023_VERSION_MAJOR 1)
set (pace2023_VERSION_MINOR 0)

# Headers
include_directories(include)
include_directories(cadical/src)

execute_process(
    COMMAND ./configure
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/cadical
    OUTPUT_VARIABLE configure
)
message(${configure})

message("Building CaDiCaL")
execute_process(
    COMMAND make
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/cadical
    OUTPUT_VARIABLE make_cadical
)
message(${make_cadical})

set(SRC_FILES
	#"${PROJECT_SOURCE_DIR}/src/cografando.cpp"
	"${PROJECT_SOURCE_DIR}/src/directed_graph.cpp"
	"${PROJECT_SOURCE_DIR}/src/graph.cpp"
	#"${PROJECT_SOURCE_DIR}/src/main.cpp"
	"${PROJECT_SOURCE_DIR}/src/sat_solver.cpp"
	"${PROJECT_SOURCE_DIR}/src/solver.cpp")

## Objetos comuns a todos os targets
add_library(common OBJECT ${SRC_FILES})

## Configuração para compilação do executável final
set(CMAKE_CXX_FLAGS "-Ofast")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
add_executable(pace $<TARGET_OBJECTS:common> "${PROJECT_SOURCE_DIR}/src/main.cpp")
target_link_libraries(pace "-L${PROJECT_SOURCE_DIR}/cadical/build -lcadical")
