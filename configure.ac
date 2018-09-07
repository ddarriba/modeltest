#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.50])
AC_INIT(modeltest-ng, 0.1.6, diego.darriba@h-its.org)
AM_INIT_AUTOMAKE([subdir-objects])

CXXFLAGS="-g -O3 -Wall"

AC_CONFIG_SRCDIR([src/modeltest.cpp])
AC_CONFIG_HEADERS([config.h])

AC_LANG(C++)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_CPP


# Checks for libraries.
AC_CHECK_LIB([m], [main])

AC_CHECK_HEADERS([libpll/pll_binary.h], [], [AC_MSG_ERROR([pll_binary.h not found])])
AC_CHECK_HEADERS([libpll/pllmod_algorithm.h], [], [AC_MSG_ERROR([pllmod_algorithm.h not found])])
AC_CHECK_HEADERS([libpll/pll_optimize.h], [], [AC_MSG_ERROR([pll_optimize.h not found])])
AC_CHECK_HEADERS([libpll/pll_tree.h], [], [AC_MSG_ERROR([pll_tree.h not found])])

AC_ARG_ENABLE(static, AS_HELP_STRING([--enable-static], [Build static binary]), [build_static=yes])
AM_CONDITIONAL(HAVE_PTHREADS, test "x$have_pthreads" = "xyes")

AM_CONDITIONAL(BUILD_STATIC, test "x$build_static" = "xyes")
if test x"$build_static" != xyes; then
  AC_CHECK_LIB([pthread], [pthread_create])
fi

# Checks for header files.
AC_CHECK_HEADERS([cassert climits cstdin fstream iomanip iostream map string vector])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([floor mkdir pow sqrt strrchr strtol])

have_mpi=no
AX_MPI([have_mpi=yes],[have_mpi=no
  if test x"$have_mpi" = xyes; then
     AC_MSG_FAILURE([MPI compiler requested, but couldn't use MPI.])
  else
     AC_MSG_WARN([No MPI compiler found, won't use MPI.])
  fi
])
AC_SUBST(MPICXX)

AM_CONDITIONAL(BUILD_MPI, test "x$have_mpi" = "xyes")

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 mpi/Makefile])

AC_OUTPUT
