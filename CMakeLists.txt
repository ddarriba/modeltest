cmake_minimum_required (VERSION 3.0 FATAL_ERROR)

###############################################################################
# CONFIGURATION
###############################################################################

set (STATIC_BUILD OFF CACHE BOOL "Build static binary")
set (ENABLE_MPI OFF CACHE BOOL "Enable MPI support")
set (ENABLE_GUI OFF CACHE BOOL "Enable QT GUI")
set (ENABLE_MULTITHREAD ON CACHE BOOL "Enable multi-threading support")
set (OSX_BUNDLE OFF CACHE BOOL "Build GUI bundle (OSX only)")

# set following options to OFF to build a portable binary
# (don't worry, libpll will still have full SIMD support!)
set (ENABLE_MODELTEST_SIMD ON CACHE BOOL "Enable SIMD instructions (non-portable but slightly faster)")
set (ENABLE_PLLMOD_SIMD ON CACHE BOOL "Enable SIMD instructions in pll-modules (non-portable but slightly faster)")

###############################################################################
# END OF CONFIGURATION
###############################################################################

project (modeltest C CXX)

set (modeltest_VERSION_MAJOR 0)
set (modeltest_VERSION_MINOR 2)
set (modeltest_VERSION_PATCH 0)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  # which compilers to use for C and C++
  set(CMAKE_C_COMPILER i686-w64-mingw32-gcc-posix)
  set(CMAKE_CXX_COMPILER i686-w64-mingw32-g++-posix)

  # here is the target environment located
  set(CMAKE_FIND_ROOT_PATH /usr/i686-w64-mingw32)
  set(HOST_ARCHITECTURE "win32")
else()
  execute_process(COMMAND uname -m OUTPUT_VARIABLE arch OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(HOST_ARCHITECTURE "${arch}")
  string(TOLOWER ${HOST_ARCHITECTURE} HOST_ARCHITECTURE)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

  if(HOST_ARCHITECTURE MATCHES "^arm64")
    set (CMAKE_OSX_DEPLOYMENT_TARGET 11.1)
  else()
    set (CMAKE_OSX_DEPLOYMENT_TARGET 10.7)
  endif()

  if (OSX_BUNDLE)
    message (STATUS "Build GUI Bundle")
    set (ENABLE_GUI ON)
  endif()

  # no support for static linking on Mac
  if (STATIC_BUILD)
    message (STATUS "ModelTest Static build disabled for Darwin")
    set (STATIC_BUILD OFF)
  endif()

  if (ENABLE_MPI)
      message (STATUS "ModelTest MPI build disabled for Darwin")
    set (ENABLE_MPI OFF)     # MPI support on Macs seems to be tricky (and unnecessary)
  endif()
else()
  if (OSX_BUNDLE)
    message (STATUS "OSX_BUNDLE disabled (wrong OS)")
    set (OSX_BUNDLE OFF)
  endif()
endif()

set (CMAKE_BUILD_TYPE RELEASE)
if ( $ENV{MT_DEBUG} )
  set (CMAKE_BUILD_TYPE DEBUG)
endif ()
message (STATUS "Building ${CMAKE_BUILD_TYPE}")

if ( $ENV{MT_VERBOSE} )
  set (CMAKE_VERBOSE_MAKEFILE ON)
  message (STATUS "ModelTest Verbose Makefile")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -mmacosx-version-min=10.7")
  set (WARN_FLAGS               "-Wall -Wno-unused-parameter")
else()
  set (WARN_FLAGS               "-Wall -Wextra -Wno-unused-parameter -Wno-unused-but-set-variable")
  set (CMAKE_CXX_FLAGS          "-std=c++11 ${WARN_FLAGS}")
endif()

set (CMAKE_CXX_FLAGS_DEBUG    "-O0 -g -ggdb3 -DDEBUG -D_GLIBCXX_DEBUG")
set (CMAKE_C_FLAGS_DEBUG      "-O0 -g -ggdb3 -D_GLIBCXX_DEBUG")
set (CMAKE_CXX_FLAGS_RELEASE  "-O3 -g -DNDEBUG")
set (CMAKE_C_FLAGS_RELEASE    "-O3 -g")

if(HOST_ARCHITECTURE MATCHES "^arm64")
  set(HAS_AVX2 false)
  set(HAS_AVX false)
  set(HAS_SSE false)
  set(ENABLE_PLLMOD_SIMD false)
else()
  if (ENABLE_MODELTEST_SIMD)
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG(-mavx2 HAS_AVX2)
    CHECK_CXX_COMPILER_FLAG(-mavx HAS_AVX)
    CHECK_CXX_COMPILER_FLAG(-msse3 HAS_SSE3)
    if(HAS_AVX2)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -D__AVX2")
    elseif(HAS_AVX)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx -D__AVX")
    elseif(HAS_SSE3)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse3 -D__SSE3")
    endif()
  endif()
endif()

if(ENABLE_PLLMOD_SIMD)
  set(PLLMOD_AUTOCONF_ARGS "--enable-avx2 --enable-avx --enable-sse")
else()
  set(PLLMOD_AUTOCONF_ARGS "")
endif()

if(ENABLE_MULTITHREAD)
  find_package(Threads REQUIRED)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMULTITHREAD -pthread")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMULTITHREAD -pthread")
endif()

if(ENABLE_GUI)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  # Find the QtWidgets library
  find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_NO_GUI_ ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
endif()

if(ENABLE_MPI)
  find_package(MPI REQUIRED)
  if(MPI_CXX_FOUND)
    include_directories(${MPI_CXX_INCLUDE_PATH})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_CXX_COMPILER_FLAGS} -DBUILD_MPI")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MPI_CXX_LINK_FLAGS}")

    # disable static build for MPI
    set(STATIC_BUILD OFF)
  endif()
endif()

message(STATUS "ModelTest Using flags: ${CMAKE_CXX_FLAGS}")

#check for minimum compiler versions
message(STATUS "ModelTest Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} => ${CMAKE_CXX_COMPILER}")

# =============================================
#   Download Dependencies
# =============================================

# Downloads dependencies if they are not there. We use a special script for this.
include( "${CMAKE_CURRENT_LIST_DIR}/libs/cmake/DownloadDependency.cmake" )

# Do not replace the hashes below manually!
SET( pll-modules_COMMIT_HASH "182ae2823fa1f7b8c67ec68f491c4adea59e25db" )
SET( libpll_COMMIT_HASH "a3146f31ea044e3dc6a52175e2981fe3643dbbdf" )

DOWNLOAD_GITHUB_DEPENDENCY( "${PROJECT_SOURCE_DIR}/libs" "pll-modules" "README.md" "ddarriba/pll-modules" ${pll-modules_COMMIT_HASH} )
DOWNLOAD_GITHUB_DEPENDENCY( "${PROJECT_SOURCE_DIR}/libs/pll-modules/libs" "libpll" "README.md" "xflouris/libpll-2" ${libpll_COMMIT_HASH} )

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

# =============================================
#         Build pll-modules
# =============================================

set( BUILD_PLLMODULES_STATIC ON   CACHE BOOL "Passing BUILD_PLLMODULES_STATIC=ON to pll-modules" )
set( BUILD_PLLMODULES_SHARED OFF  CACHE BOOL "Passing BUILD_PLLMODULES_SHARED=OFF to pll-modules" )
set( MODELTEST_LOCALDEPS_DIR ${PROJECT_BINARY_DIR}/localdeps )
set( PLLMOD_SRC "${PROJECT_SOURCE_DIR}/libs/pll-modules/src" )
set( LIBPLL_SRC "${PROJECT_SOURCE_DIR}/libs/pll-modules/libs/libpll/src" )

if ( ENABLE_GUI )
  set(CMAKE_AUTOUIC OFF)
  set(CMAKE_AUTOMOC OFF)
endif()
add_subdirectory( ${PROJECT_SOURCE_DIR}/libs/pll-modules )
if ( ENABLE_GUI )
  set(CMAKE_AUTOUIC ON)
  set(CMAKE_AUTOMOC ON)
endif()

file(COPY ${PLLMOD_SRC}/pllmod_common.h
          ${PLLMOD_SRC}/util/pllmod_util.h
          ${PLLMOD_SRC}/algorithm/pllmod_algorithm.h
          ${PLLMOD_SRC}/binary/pll_binary.h
          ${PLLMOD_SRC}/msa/pll_msa.h
          ${PLLMOD_SRC}/optimize/pll_optimize.h
          ${PLLMOD_SRC}/tree/pll_tree.h
          ${LIBPLL_SRC}/pll.h
          DESTINATION ${MODELTEST_LOCALDEPS_DIR}/include/libpll)

# =============================================
#         Recurse and compile main code
# =============================================

include_directories(${MODELTEST_LOCALDEPS_DIR}/include)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)

# =============================================
#         Packaging
# =============================================

set(CPACK_GENERATOR "TGZ;STGZ;DEB")
set(CPACK_SOURCE_GENERATOR "TGZ;STGZ")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Diego Darriba")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libqt5widgets5, libqt5core5a, libqt5gui5")

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "GAC")
set(CPACK_VERBATIM_VARIABLES true)
set(CPACK_PACKAGE_VERSION_MAJOR ${modeltest_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${modeltest_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${modeltest_VERSION_PATCH})

set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

set(CPACK_SOURCE_IGNORE_FILES "${CMAKE_SOURCE_DIR}/build/;${CMAKE_SOURCE_DIR}/legacy/;${CMAKE_SOURCE_DIR}/.git/;${CMAKE_SOURCE_DIR}/manual/")

include(CPack)
