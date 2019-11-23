cmake_minimum_required(VERSION 3.0.0)
project(fpar VERSION 0.1.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors -Wall -Wno-unused-variable -Wno-unused-result -std=c++17")

# add debug flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -DDEBUG")

# add stdlib to clang++
# TODO: check that the libc++ is installed
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -Wsign-compare")
  message(STATUS "is clang, libc++ added")
endif()

# Find Openmp
find_package(OpenMP)
if(NOT OPENMP_FOUND)
  message(FATAL_ERROR "OPENMP library not found")
endif()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

# Find/Download Immer
find_path(IMMER_INCLUDE immer/box.hpp)
if(NOT IMMER_INCLUDE)
# reference: https://foonathan.net/2016/07/cmake-dependency-handling/
    message("Download 'immer'")
    execute_process(COMMAND git submodule update --init -- externals/immer
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set(IMMER_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/externals/immer)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(CPack)

# Compile Library
add_library(fpar src/all.cpp)
target_include_directories(fpar PUBLIC ./include ${IMMER_INCLUDE})
target_link_libraries(fpar ${OPENMP} ${PTHREAD})

# Compile Benches
add_executable (matrix_mul benchs/matrix_mul.cpp)
target_link_libraries(matrix_mul fpar ${OPENMP} ${PTHREAD})

add_executable (sort_all benchs/sort_all.cpp)
target_link_libraries(sort_all fpar ${OPENMP} ${PTHREAD})

# Compile Examples
add_executable (toy_example examples/toy_example.cpp)
target_link_libraries(toy_example fpar ${OPENMP} ${PTHREAD})
