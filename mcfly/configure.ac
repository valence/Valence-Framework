#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ()
AC_INIT()


# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_MAKE_SET

# If we have cpp enable bindings
if test -z "$CXX"; then
    enable_cpp_bindings="set";
fi


# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_TIME
AC_CHECK_HEADERS([stdlib.h stddef.h fcntl.h termios.h])


# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T


# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_SELECT_ARGTYPES
AC_CHECK_FUNCS([memset select mkfifo strchr])


# Flags
FLAGS="-Wall -pedantic"
CFLAGS="-std=c99"
CXXFLAGS="-std=c++98"

AC_ARG_ENABLE([cpp-bindings],
              [AC_HELP_STRING([--enable-cpp-bindings], 
                              [Enable C++ bindings])],
              [USE_CPP_BINDINGS=1],
              [USE_CPP_BINDINGS=0]) # <---- Default

AC_ARG_ENABLE([debug],
              [AC_HELP_STRING([--enable-debug], [Enable debugging build])],
              [FLAGS="$FLAGS -DDEBUG"])

AC_ARG_ENABLE([debug-annoy],
              [AC_HELP_STRING([--enable-debug-annoy],
                              [Enable really verbose debugging])],
              [FLAGS="$FLAGS -DDEBUG -DDEBUG_ANNOY"])

# If we never asked for cpp bindings OR we couldnt find a compiler disable cpp
if test "${enable_cpp_bindings+set}" != set || test -z "${CXX}"; then
    USE_CPP_BINDINGS=0
fi

# If we have not enabled debug set common flags
if test "${enable_debug+set}" != set && test "${enable_debug_annoy+set}" != set; then
    FLAGS="$FLAGS -O3"
else
    FLAGS="$FLAGS -g3"
fi

AC_SUBST(FLAGS)
AC_SUBST(CFLAGS)
AC_SUBST(CXXFLAGS)
AC_SUBST(USE_CPP_BINDINGS)


# Cholesteral
CLIBS="-ldl -lpthread"
AC_SUBST(CLIBS)

AC_CONFIG_FILES([Makefile
                 srclib/Makefile
                 tests/Makefile
                 tools/Makefile
                 tools/simulator/Makefile
                 modules/Makefile
                 modules/dummy_module/Makefile
                 modules/elm327_module/Makefile
                 bindings/Makefile
                 bindings/cpp/Makefile
                 bindings/cpp/srclib/Makefile
                 bindings/cpp/tests/Makefile])
AC_OUTPUT
