#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ()
AC_INIT()

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET

# Checks for libraries.

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_TIME
AC_CHECK_HEADERS([stdlib.h stddef.h fcntl.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_SELECT_ARGTYPES
AC_CHECK_FUNCS([memset select mkfifo])

# Flags
CFLAGS="-g3 -Wall"
AC_SUBST(CFLAGS)

# Cholesteral
CLIBS="-ldl -lpthread"
AC_SUBST(CLIBS)

AC_CONFIG_FILES([Makefile
                 srclib/Makefile
                 modules/Makefile
                 tests/Makefile])
AC_OUTPUT