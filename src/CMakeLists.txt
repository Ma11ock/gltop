set(GLTOP_BINARY_FINAL gltop)

enable_language(C)
enable_language(CXX)
set(CMAKE_C_VERSION 99)
set(CMAKE_CXX_VERSION 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_policy(SET CMP0075 NEW)


set(
  GLTOP_SOURCES
  gltop.cpp
  )

set(
  GLTOP_HEADERS
  gltop.hpp
  )

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(
  ${GLTOP_BINARY_FINAL}
  ${GLTOP_SOURCES}
  ${GLTOP_HEADERS}
  )


target_compile_features(gltop PRIVATE cxx_std_17)
target_compile_features(gltop PRIVATE c_std_99)
