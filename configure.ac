# Process this file with autoconf to produce a configure script.
AC_INIT([thash],[1],[bonelli@antifork.org])
AC_CONFIG_SRCDIR([thash.h])
AC_CONFIG_HEADER([config.h])

AC_SUBST(INCLS)
AC_SUBST(regsize)

# Checks for programs.
AC_PROG_CC
AC_PATH_PROG(PATH_CC, cc)

if test $ac_cv_prog_gcc = yes; then
    CCOPTS='-O3 -funroll-loops -fomit-frame-pointer -pipe'
    CFLAGS="$CCOPTS"
fi


# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_CHECK_FUNCS([memset munmap])

AC_CHECK_LIB(dl, dlopen,)
AC_CHECK_LIB(m, log,,AC_MSG_ERROR(limath not found.))

# regsize 
AC_ARG_WITH(regsize,
[  --with-regsize=Mbyte    set the collision's register size (default=64 Mbyte)],
[
        regsize=$withval
],[
	regsize=64
])

AC_REPLACE_FUNCS([strlcpy strlcat])

# check for headers

AH_TEMPLATE([HAVE_BSD_SYS_QUEUE_H],
[define if the sys/queue.h BSD header is present
])

AC_CHECK_HEADERS(sys/queue.h, [AC_EGREP_CPP(have_bsd_sys_queue, [
#include <sys/queue.h>
#ifdef SLIST_HEAD
  have_bsd_sys_queue
#endif
], AC_DEFINE([HAVE_BSD_SYS_QUEUE_H]))])    


AC_CONFIG_FILES([Makefile])
AC_OUTPUT

rm -f local.h
echo "thash has been configured with the following options:"
echo "  compiler   : $CC"
echo "  CFLAGS     : $CFLAGS"
echo "  lib        : $LIBS"
echo "  defs       : $DEFS"
echo "  include    : $INCLS"
echo "  prefix     : $prefix"
echo "  exec_prefix: $exec_prefix"
echo "  bindir     : $bindir"
echo "  mandir     : $mandir"
echo "  datadir    : $datadir"
echo "  regsize    : $regsize Mbyte ( configure --with-regsize=Mbyte )"
