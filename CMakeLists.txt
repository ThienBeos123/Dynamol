cmake_minimum_required(VERSION 3.10)
project(Dynamol C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set compiler flags for suppressing warnings during development
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")

# Add include directories (so header files can be found)
include_directories(${CMAKE_SOURCE_DIR}/system)
include_directories(${CMAKE_SOURCE_DIR}/intrinsics)
include_directories(${CMAKE_SOURCE_DIR}/calculation_algorithms/Headers)
include_directories(${CMAKE_SOURCE_DIR}/internal_utils)
include_directories(${CMAKE_SOURCE_DIR}/big_numbers)

# =====================================================
# BigInt Executable (main test)
# =====================================================
add_executable(bigInt_casual_comp
    big_numbers/bigInt.c
    internal_utils/bigNum_utils.c
    internal_utils/misc_utils.c
    intrinsics/limb_math.c
    calculation_algorithms/src/mul.c
    calculation_algorithms/src/div.c
)

target_compile_options(bigInt_casual_comp PRIVATE -Wno-unused-function)

# =====================================================
# BigInt_IO Executable (for future use)
# =====================================================
# Uncomment this when you're ready to test bigInt_io.c
# add_executable(bigInt_io_test
#     big_numbers/bigInt_io.c
#     internal_utils/bigNum_utils.c
#     intrinsics/limb_math.c
# )

# =====================================================
# Other BigNumber Types (for future use)
# =====================================================
# add_executable(bigInt128_test
#     big_numbers/someInt128File.c
#     internal_utils/bigNum_utils.c
#     intrinsics/limb_math.c
# )
