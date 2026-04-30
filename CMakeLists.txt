cmake_minimum_required(VERSION 3.14)
project(OrderBookEngine VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0 -fsanitize=address,undefined")

# ── Core library ────────────────────────────────────────────────
add_library(order_book_lib
    src/order_book.cpp
    src/order.cpp
    src/market_data_parser.cpp
    src/memory_pool.cpp
    src/benchmark.cpp
)
target_include_directories(order_book_lib PUBLIC include)

# ── Main executable ─────────────────────────────────────────────
add_executable(order_book src/main.cpp)
target_link_libraries(order_book PRIVATE order_book_lib)

# ── Google Test ─────────────────────────────────────────────────
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(run_tests
    tests/test_order_book.cpp
    tests/test_memory_pool.cpp
    tests/test_parser.cpp
)
target_link_libraries(run_tests PRIVATE order_book_lib GTest::gtest_main)