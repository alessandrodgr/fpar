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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

# Find/Download Immer
# reference: https://foonathan.net/2016/07/cmake-dependency-handling/
find_path(IMMER_INCLUDE immer/box.hpp)
if(IMMER_INCLUDE)
  add_library(immer INTERFACE)
  target_include_directories(immer INTERFACE ${IMMER_INCLUDE})
else()
  message("Unable to find 'immer', will be downloaded...")
  execute_process(COMMAND git submodule update --init -- externals/immer
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  # set IMMER_INCLUDE properly
  set(IMMER_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/externals/immer)

  # also install it
  install(DIRECTORY ${IMMER_INCLUDE}/immer DESTINATION include)

  # for convenience setup a target
  add_library(immer INTERFACE)
  target_include_directories(immer INTERFACE
                             $<BUILD_INTERFACE:${IMMER_INCLUDE}>
                             $<INSTALL_INTERFACE:include/immer>)

  # need to export target as well
  install(TARGETS immer EXPORT my_export_set DESTINATION include)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(CPack)

if(COMPILE_BIN)
  # Compile Benches
  add_executable (matrix_mul benchs/matrix_mul.cpp)
  target_include_directories(matrix_mul PUBLIC ./src)
  target_link_libraries(matrix_mul immer)

  add_executable (sort_all benchs/sort_all.cpp)
  target_include_directories(sort_all PUBLIC ./src)
  target_link_libraries(sort_all immer)

  # Compile Examples
  add_executable (toy_example examples/toy_example.cpp)
  target_include_directories(toy_example PUBLIC ./src)
  target_link_libraries(toy_example immer)
endif()

# Install Library
install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/fpar
        DESTINATION include
        FILES_MATCHING PATTERN "*.hpp")
