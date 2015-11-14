#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.50])
AC_INIT(modeltest, 1.0.0, ddarriba@udc.es)
AM_INIT_AUTOMAKE([subdir-objects]) # (partest, 1.0)

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
AC_CHECK_LIB([pthread], [pthread_create])

#AC_SEARCH_LIBS([pll_partition_create], [pll], [], [
#  AC_MSG_ERROR([PLL library not found])
#])
#AC_CHECK_LIB([pll_optimize], [pll_compute_empirical_frequencies], [], [
#  AC_MSG_ERROR([PLL_Optimize library not found])
#])

PKG_CHECK_MODULES(QT4, [QtCore QtGui >= 4.4.0], [
  AC_PATH_PROGS(MOC, [moc-qt4 moc], moc,`eval $PKG_CONFIG --variable=exec_prefix QtCore`/bin)
  AC_PATH_PROG(RCC, rcc, rcc,`eval $PKG_CONFIG --variable=exec_prefix QtCore`/bin)
  AC_PATH_PROGS(UIC, [uic-qt4 uic], uic,`eval $PKG_CONFIG --variable=exec_prefix QtCore`/bin)
], [
  AC_MSG_ERROR([Qt 4 library not found])
])

AC_MSG_CHECKING(for SSE in current arch/CFLAGS)
AC_LINK_IFELSE([
AC_LANG_PROGRAM([[
#include <xmmintrin.h>
__m128 testfunc(float *a, float *b) {
  return _mm_add_ps(_mm_loadu_ps(a), _mm_loadu_ps(b));
}
]])],
[
have_sse=yes
],
[
have_sse=no
]
)
AC_MSG_RESULT($have_sse3)


AC_ARG_ENABLE(avx, [AC_HELP_STRING([--enable-avx],[enable AVX optimizations])], have_avx=$enableval, have_avx=no)
if test "$have_avx" = "yes"; then
        AC_DEFINE(HAVE_AVX,1,[Define to enable AVX optimizations.])
	if test "$PRECISION" != "d" -a "$PRECISION" != "s"; then
		AC_MSG_ERROR([AVX requires single or double precision])
	fi
fi

AM_CONDITIONAL(HAVE_AVX, test "$have_avx" = "yes")
AM_CONDITIONAL(HAVE_PTHREADS, test "x$have_pthreads" = "xyes")
AM_CONDITIONAL(HAVE_SSE, test "x$have_sse" = "xyes")

# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([stdlib.h string.h unistd.h iostream.h fstream.h iomanip math.h assert.h pll.h])

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
                 src/Makefile])
AC_OUTPUT
