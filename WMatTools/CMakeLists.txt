cmake_minimum_required(VERSION 3.14)
project(WMatTools C CXX)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set C standard instead of C++ standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  tests
  tests.cc
  WMatInt.c
  WMatIntTools.c
)
target_link_libraries(
  tests
  GTest::gtest_main
  Threads::Threads
)

include(GoogleTest)
gtest_discover_tests(tests)

# Speed tests are a separate executable and not registered with ctest,
# so the normal test run stays fast. Run build\speed_tests.exe directly.
add_executable(
  speed_tests
  speed_tests.cc
  WMatInt.c
)
target_link_libraries(
  speed_tests
  GTest::gtest_main
  Threads::Threads
)
target_compile_definitions(
  speed_tests
  PRIVATE WMAT_DATA_DIR="${CMAKE_SOURCE_DIR}/data"
)
# Time optimized code, otherwise the default build has no optimization
target_compile_options(speed_tests PRIVATE -O2)

# Eigen is header only, so it just needs its include path.
set(EIGEN_DIR "C:/Users/tim_w/Documents/eigen" CACHE PATH "Path to the Eigen source tree")
target_include_directories(speed_tests PRIVATE ${EIGEN_DIR})
# Turn off Eigen's internal bounds checks so it isn't timed with them.
# This does not affect the assert() calls in WMatInt.c.
target_compile_definitions(speed_tests PRIVATE EIGEN_NO_DEBUG)

# OpenMP lets Eigen multithread its matrix product. Only C++ files get
# -fopenmp, so WMatInt.c is compiled exactly as before.
find_package(OpenMP REQUIRED COMPONENTS CXX)
target_link_libraries(speed_tests OpenMP::OpenMP_CXX)
# CMake 3.14 with MinGW only applies -fopenmp when compiling, so also
# pass it to the linker to pull in the OpenMP runtime (libgomp).
target_link_options(speed_tests PRIVATE ${OpenMP_CXX_FLAGS})