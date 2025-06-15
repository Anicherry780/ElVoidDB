cmake_minimum_required(VERSION 3.15)
project(elvoiddb LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# ------------------------------------------------------------------
#  Source list — every .cpp in src/ EXCEPT main.cpp (CLI entry point)
# ------------------------------------------------------------------
file(GLOB ELVOIDDB_SRC CONFIGURE_DEPENDS src/*.cpp)
list(REMOVE_ITEM ELVOIDDB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

# Core library (query engine, storage, parser …)
add_library(elvoiddb_core ${ELVOIDDB_SRC})
# Headers live under include/
target_include_directories(elvoiddb_core PUBLIC include)

# CLI executable
add_executable(elvoiddb src/main.cpp)
# Link against the core and expose headers to the CLI build
target_link_libraries(elvoiddb PRIVATE elvoiddb_core)
# (include path already provided transitively by PUBLIC on elvoiddb_core)
