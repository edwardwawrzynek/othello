cmake_minimum_required(VERSION 3.17)
project(othello)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Release)

set(CCFLAGS -Wall -Wextra -pedantic -Wno-unused-parameter)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(SOURCES src/bitboard.cpp src/evaluator.cpp src/minimax.cpp src/hash_table.cpp src/stats.cpp src/api.cpp)
set(DRIVER src/driver.cpp)

find_package(nlohmann_json 3.2.0 REQUIRED)
include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/whoshuu/cpr.git GIT_TAG c8d33915dbd88ad6c92b258869b03aba06587ff9) # the commit hash for 1.5.0
FetchContent_MakeAvailable(cpr)

include_directories(src)

add_executable(othello ${SOURCES} ${DRIVER})
target_compile_options(othello PRIVATE ${CCFLAGS})
target_link_libraries(othello PRIVATE nlohmann_json::nlohmann_json cpr::cpr)

include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

if( supported )
    message(STATUS "IPO / LTO supported")
    set_property(TARGET othello PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif()

