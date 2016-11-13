#AC_PREREQ(2.61)
AC_INIT([sequoia], [0.9], [langer@cs.rwth-aachen.de])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall foreign subdir-objects])

# order is important
AC_PROG_CXX
AC_PROG_CC
AC_LANG([C++])
AM_PROG_LEX
AC_PROG_YACC
AM_PROG_AR
AC_PROG_LIBTOOL

dnl boost
BOOST_REQUIRE([1.35])
dnl order is somewhat important in case we use staged libs
BOOST_SYSTEM
BOOST_GRAPH
BOOST_HASH
BOOST_FILESYSTEM
BOOST_UTILITY
BOOST_TOKENIZER
BOOST_TUPLE
BOOST_REGEX
BOOST_THREADS
BOOST_FIND_HEADER([boost/timer/timer.hpp],[
        HAVE_BOOST_TIMER="no"
        AC_DEFINE([HAVE_BOOST_TIMER],[0],[define 1 if boost.Timer is available])
    ],[
        AC_DEFINE([HAVE_BOOST_TIMER],[1],[define 1 if boost.Timer is available])
        HAVE_BOOST_TIMER="yes"
    ])
if test "x$HAVE_BOOST_TIMER" = "xyes"; then
    BOOST_CHRONO
    BOOST_TIMER
fi

dnl Intel Thread Building Blocks
AX_INTEL_TBB([4.0])
CPPFLAGS="$CPPFLAGS $TBB_CPPFLAGS"
LDFLAGS="$LDFLAGS $TBB_LDFLAGS"

AC_DEFUN([SEQUOIA_STATIC_BUILD],
  [AC_ARG_ENABLE([build-static],
     [AC_HELP_STRING([--build-static],
               [Build statically linked libraries and binaries [no]])],
     [
	AC_DEFINE([SEQUOIA_STATIC_BUILD],[1],[define 1 to indicate static build])
	LDFLAGS="$LDFLAGS -all-static"
     ],
     [ ]
)])

dnl create
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
Makefile
src/Makefile
tools/Makefile
test/Makefile
])
AC_CONFIG_SUBDIRS([contrib/sparsehash-2.0.3])
AC_OUTPUT
