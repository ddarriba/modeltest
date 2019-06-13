message(STATUS "Building dependencies in: ${MODELTEST_LOCALDEPS_DIR}")

set(CMAKE_SYSTEM_NAME Windows)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

# which compilers to use for C and C++
set(CMAKE_C_COMPILER i686-w64-mingw32-gcc-posix)
set(CMAKE_CXX_COMPILER i686-w64-mingw32-g++-posix)

# here is the target environment located
set(CMAKE_FIND_ROOT_PATH /usr/i686-w64-mingw32)

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
#set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
endif()

# build LIBPLL and LIBPLL-MODULES
add_custom_command(
    OUTPUT ${MODELTEST_LOCALDEPS_DIR}/lib/libpll.a
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/pll-modules
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/pll-modules/install-with-libpll.sh ${MODELTEST_LOCALDEPS_DIR} '' ${PLLMOD_AUTOCONF_ARGS}
)

add_custom_target(libpll ALL
    DEPENDS ${MODELTEST_LOCALDEPS_DIR}/lib/libpll.a
)
