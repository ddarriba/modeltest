cmake_minimum_required (VERSION 2.8.10 FATAL_ERROR)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)

set (USE_PTHREADS ON CACHE BOOL "Enable multi-threading support (PTHREADS)")
set (USE_MPI OFF CACHE BOOL "Enable MPI support")
set (USE_GUI OFF CACHE BOOL "Enable QT GUI")

# set both following options to OFF to build a portable binary 
# (don't worry, libpll will still have full SIMD support!)
set (ENABLE_MODELTEST_SIMD OFF CACHE BOOL "Enable SIMD instructions (non-portable but slightly faster)")
set (ENABLE_PLLMOD_SIMD OFF CACHE BOOL "Enable SIMD instructions in pll-modules (non-portable but slightly faster)")

# build a static binary
set(STATIC_BUILD OFF CACHE BOOL "Build static binary")

set(ENABLE_PTHREADS ${USE_PTHREADS})

project (modeltest-ng C CXX)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # no support for static linking on Mac
  set(STATIC_BUILD OFF)
  # MPI support on Macs seems to be tricky (and unnecessary)
  set(ENABLE_MPI OFF)
else()
  set(ENABLE_GUI ${USE_GUI})
  set(ENABLE_MPI ${USE_MPI})
endif()

#check for minimum compiler versions
message(STATUS "Compier: ${CMAKE_CXX_COMPILER_ID}")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(MODELTEST_COMPILER_TARGET_VERSION "4.8")
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS MODELTEST_COMPILER_TARGET_VERSION)
    message (FATAL_ERROR "${CMAKE_CXX_COMPILER_ID} compiler too old! Minimum required: ${MODELTEST_COMPILER_TARGET_VERSION}")
  endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(MODELTEST_COMPILER_TARGET_VERSION "3.8")
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS MODELTEST_COMPILER_TARGET_VERSION)
    message (FATAL_ERROR "${CMAKE_CXX_COMPILER_ID} compiler too old! Minimum required: ${MODELTEST_COMPILER_TARGET_VERSION}")
  endif()
endif()

set (modeltest-ng_VERSION_MAJOR 0)
set (modeltest-ng_VERSION_MINOR 5)

set (CMAKE_BUILD_TYPE DEBUG)
#set (CMAKE_BUILD_TYPE RELEASE)
# set (CMAKE_VERBOSE_MAKEFILE ON)

message (STATUS "Building ${CMAKE_BUILD_TYPE}")

set (WARN_FLAGS               "-Wall -Wextra")
set (CMAKE_CXX_FLAGS          "-std=c++11 ${WARN_FLAGS}")

set (CMAKE_CXX_FLAGS_DEBUG    "-g")
set (CMAKE_CXX_FLAGS_RELEASE  "-O3 -DNDEBUG")

if (ENABLE_MODELTEST_SIMD)
  include(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG(-mavx HAS_AVX)
  CHECK_CXX_COMPILER_FLAG(-msse3 HAS_SSE3)
  if(HAS_AVX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx -D__AVX")
  elseif(HAS_SSE3)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse3 -D__SSE3")
  endif()
endif()

if(ENABLE_PLLMOD_SIMD)
  set(PLLMOD_AUTOCONF_ARGS "--enable-avx --enable-sse")
else()
  set(PLLMOD_AUTOCONF_ARGS "")
endif()

if(ENABLE_PTHREADS)
  find_package(Threads REQUIRED)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPTHREADS -pthread")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPTHREADS -pthread")
endif()

if(ENABLE_GUI)
  find_package(Qt5Widgets)
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_NO_GUI_ ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
endif()

if(ENABLE_MPI)
  find_package(MPI)
  if(MPI_CXX_FOUND)
    set(CMAKE_CXX_COMPILER "${MPI_CXX_COMPILER}")
    include_directories(${MPI_CXX_INCLUDE_PATH})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_CXX_COMPILER_FLAGS} -DBUILD_MPI")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MPI_CXX_LINK_FLAGS}")

    # disable static build for MPI
    set(STATIC_BUILD OFF)
  endif()
endif()

message(STATUS "Using flags: ${CMAKE_CXX_FLAGS}")

set(pll_dir ${PROJECT_SOURCE_DIR}/libs/pll-modules/libs/libpll/src)
set(pll_binary_dir ${PROJECT_SOURCE_DIR}/libs/pll-modules/src/binary)
set(pll_optimize_dir ${PROJECT_SOURCE_DIR}/libs/pll-modules/src/optimize)
set(pll_msa_dir ${PROJECT_SOURCE_DIR}/libs/pll-modules/src/msa)

# build dependencies
set(MODELTEST_LOCALDEPS_DIR ${PROJECT_BINARY_DIR}/localdeps)
add_subdirectory(${PROJECT_SOURCE_DIR}/libs)

include_directories(${MODELTEST_LOCALDEPS_DIR}/include)

add_subdirectory(${PROJECT_SOURCE_DIR}/src)
