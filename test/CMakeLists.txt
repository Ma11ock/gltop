
enable_testing()

find_package(Threads REQUIRED)
find_package(GTest REQUIRED)
enable_language(CXX)

project(gltop_test)

set(
    GLTOP_TEST_SRCS
    main.cpp
    )

add_executable(
    gltop_test
    ${GLTOP_TEST_SRCS}
    ${GLTOP_HEADERS}
    )

target_link_libraries(
    gltop_test
    gltop
    gtest
    gtest_main
    pthread
    )


# "make check" to compile and run the tests.
add_test(
    NAME    gltop_test
    COMMAND gltop_test
)

include_directories(${CMAKE_BINARY_TEST_DIR}/src)
target_compile_features(gltop_test PRIVATE cxx_std_17)
target_compile_features(gltop_test PRIVATE c_std_99)


add_custom_target(check COMMAND gltop_test
  DEPENDS gltop_test)

