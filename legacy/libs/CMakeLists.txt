message(STATUS "Building dependencies in: ${MODELTEST_LOCALDEPS_DIR}")

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  # which compilers to use for C and C++
  set(CMAKE_C_COMPILER i686-w64-mingw32-gcc-posix)
  set(CMAKE_CXX_COMPILER i686-w64-mingw32-g++-posix)

  # here is the target environment located
  set(CMAKE_FIND_ROOT_PATH /usr/i686-w64-mingw32)
endif()

if (USE_LIBPLL_CMAKE)
  add_subdirectory(pll-modules)

  set(PLLMOD_SRC "${CMAKE_CURRENT_SOURCE_DIR}/pll-modules/src")
  set(LIBPLL_SRC "${CMAKE_CURRENT_SOURCE_DIR}/pll-modules/libs/libpll/src")

  file(COPY ${PLLMOD_SRC}/pllmod_common.h
            ${PLLMOD_SRC}/util/pllmod_util.h
            ${PLLMOD_SRC}/algorithm/pllmod_algorithm.h
            ${PLLMOD_SRC}/binary/pll_binary.h
            ${PLLMOD_SRC}/msa/pll_msa.h
            ${PLLMOD_SRC}/optimize/pll_optimize.h
            ${PLLMOD_SRC}/tree/pll_tree.h
            ${LIBPLL_SRC}/pll.h
            DESTINATION ${MODELTEST_LOCALDEPS_DIR}/include/libpll)

#target_include_directories(modeltest_module PRIVATE ${MODELTEST_LIBPLL_HEADERS})

else()
  add_custom_command(
    OUTPUT ${MODELTEST_LOCALDEPS_DIR}/lib/libpll.a
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/pll-modules
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/pll-modules/install-with-libpll.sh ${MODELTEST_LOCALDEPS_DIR} '' ${PLLMOD_AUTOCONF_ARGS}
  )

  add_custom_target(libpll ALL
    DEPENDS ${MODELTEST_LOCALDEPS_DIR}/lib/libpll.a
  )
endif()
