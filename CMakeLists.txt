cmake_minimum_required(VERSION 3.15)
project(astrodb LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# ------------------------------------------------------------------
#  Source list — every .cpp in src/ EXCEPT main.cpp (CLI entry point)
# ------------------------------------------------------------------
file(GLOB ASTRODB_SRC CONFIGURE_DEPENDS src/*.cpp)
list(REMOVE_ITEM ASTRODB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

# Core library (query engine, storage, parser …)
add_library(astrodb_core ${ASTRODB_SRC})
# Headers live under include/
target_include_directories(astrodb_core PUBLIC include)

# CLI executable
add_executable(astrodb src/main.cpp)
# Link against the core and expose headers to the CLI build
target_link_libraries(astrodb PRIVATE astrodb_core)
# (include path already provided transitively by PUBLIC on astrodb_core)
