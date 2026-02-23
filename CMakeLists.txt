cmake_minimum_required(VERSION 3.10)
project(Dynamol C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set compiler flags for suppressing warnings during development
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")

# Add include directories (so header files can be found)
include_directories(${CMAKE_SOURCE_DIR}/system)
include_directories(${CMAKE_SOURCE_DIR}/sconfigs)
include_directories(${CMAKE_SOURCE_DIR}/intrinsics)
include_directories(${CMAKE_SOURCE_DIR}/calculation_algorithms/Headers)
include_directories(${CMAKE_SOURCE_DIR}/internal_utils)
include_directories(${CMAKE_SOURCE_DIR}/big_numbers)

# =====================================================
# BigInt Executable (compilation test)
# =====================================================
add_executable(bigInt
    big_numbers/bigInt.c
    big_numbers/bigInt_io.c
    internal_utils/bigNum_utils.c
    internal_utils/misc_utils.c
    internal_utils/str_parse.c
    intrinsics/limb_math.c
    calculation_algorithms/src/mul.c
    calculation_algorithms/src/div.c
    calculation_algorithms/src/num_theory.c
)

target_compile_options(bigInt PRIVATE -Wno-unused-function)
target_sources(bigInt PRIVATE ztest/main.c)
