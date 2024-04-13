cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Project name
project(pace2024)
# Versions
set (pace2024_VERSION_MAJOR 1)
set (pace2024_VERSION_MINOR 0)

# Headers
include_directories(include)

set(SRC_FILES
	"${PROJECT_SOURCE_DIR}/src/graph.cpp"
  # "${PROJECT_SOURCE_DIR}/src/sat_solver.cpp"
  # "${PROJECT_SOURCE_DIR}/src/solver.cpp"
)

# Object library
add_library(common OBJECT ${SRC_FILES})

# Final configs
set(CMAKE_CXX_FLAGS "-Ofast")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
add_executable(pace $<TARGET_OBJECTS:common> "${PROJECT_SOURCE_DIR}/src/main.cpp")
# target_link_libraries(pace "-L${PROJECT_SOURCE_DIR}/cadical/build -lcadical")
