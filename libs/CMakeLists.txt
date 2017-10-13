
message(STATUS "Building dependencies in: ${MODELTEST_LOCALDEPS_DIR}")

# build LIBPLL and LIBPLL-MODULES
add_custom_command(
    OUTPUT ${MODELTEST_LOCALDEPS_DIR}/lib/libpll.a
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/pll-modules
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/pll-modules/install-with-libpll.sh ${MODELTEST_LOCALDEPS_DIR} '' ${PLLMOD_AUTOCONF_ARGS}
)

add_custom_target(libpll ALL
    DEPENDS ${MODELTEST_LOCALDEPS_DIR}/lib/libpll.a
)
