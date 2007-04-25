dnl $Id: antlr.m4 1573 2007-02-06 15:30:33Z sjoyeux $
dnl $Rev: 1573 $

AC_DEFUN([CLBS_CHECK_ANTLR], [
    # Check for antlr-config
    AC_ARG_VAR(ANTLR_CONFIG, [the antlr-config development script])
    if test -z "$ANTLR_CONFIG"; then
        AC_PATH_PROG(ANTLR_CONFIG, antlr-config, no)
    fi
    if test "x$ANTLR_CONFIG" = "xno"; then
        AC_MSG_ERROR([cannot find the antlr-config tool])
    fi

    ac_antlr_exec_prefix=`$ANTLR_CONFIG --exec-prefix`/
    ac_antlr_bindir="$ac_antlr_exec_prefix/bin"

    AC_ARG_VAR(ANTLR, [the Antlr tool])
    if test -z "$ANTLR"; then
        AC_PATH_PROGS(ANTLR, [antlr cantlr], no, $ac_antlr_bindir)
    fi
    if test "x$ANTLR" = "xno"; then
        AC_MSG_ERROR([cannot find the antlr tool])
    fi


    ANTLR_CFLAGS=`$ANTLR_CONFIG --cflags`
    ANTLR_LIBS=`$ANTLR_CONFIG --libs`

    AC_MSG_CHECKING([for a libantlr-pic library])
    antlr_pic=`echo $ANTLR_LIBS | sed 's/libantlr\.a/libantlr-pic.a/'`
    if test -f "$antlr_pic"; then
        ANTLR_LIBS="$antlr_pic"
        AC_MSG_RESULT([$ANTLR_LIBS])
    else
        AC_MSG_RESULT(no)
        AC_MSG_WARN([cannot find libantlr-pic.a. The build may fail if your distribution did not build libantlr.a with PIC enabled])
    fi

    AC_SUBST(ANTLR_CFLAGS)

    # Transform <path>/libantlr.a into -L<path> -lantlr, which is more libtool friendly
    antlr_libdir=`dirname $ANTLR_LIBS`
    antlr_libname=`echo "$ANTLR_LIBS" | sed "s#$antlr_libdir/lib## ; s#\.a##"`
    ANTLR_LIBS="\"-L$antlr_libdir\" \"-l$antlr_libname\""
    AC_SUBST(ANTLR_LIBS)
])
