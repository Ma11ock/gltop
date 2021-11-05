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
  proc.cpp
  )

set(
  GLTOP_HEADERS
  gltop.hpp
  )

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(
  ${GLTOP_BINARY_FINAL}
  ${GLTOP_SOURCES}
  ${GLTOP_HEADERS}
  )

target_link_libraries(gltop PRIVATE GL)
target_link_libraries(gltop PRIVATE GLU)
target_link_libraries(gltop PRIVATE glut)
target_link_libraries(gltop PRIVATE m)
target_link_libraries(gltop PRIVATE GLEW)
target_link_libraries(gltop PRIVATE procps)

target_compile_features(gltop PRIVATE cxx_std_17)
target_compile_features(gltop PRIVATE c_std_99)

add_custom_target(run
    COMMAND gltop
    DEPENDS gltop
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)
