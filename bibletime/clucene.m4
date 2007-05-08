##	-*- autoconf -*-
dnl This file was created by Lee Carpenter <elc@carpie.net>
dnl Later modified by Martin Gruner <mgruner@crosswire.org>
dnl It provides macros for the autoconf package to find the CLucene library on your system.

dnl ----------------------------------------------------------------------
dnl		Check for clucene installation
dnl             First argument specifies whether or not to link
dnl             CLucene statically
dnl ----------------------------------------------------------------------
AC_DEFUN(AC_CHECK_CLUCENE,
[
dnl AC_MSG_CHECKING([for a clucene installation])

dnl The option for the configure script
AC_ARG_WITH(clucene-dir,
[  --with-clucene-dir=DIR     Path where CLucene is installed (default=/usr) ],
[
  ac_clucene_dir=$withval
],ac_clucene_dir=/usr
)

AC_ARG_ENABLE(static-clucene,
[  --enable-static-clucene    Link to the static CLucene library],
 	ac_static_clucene="YES",
 	[ ac_static_clucene="$1" ]
)

dnl try to find CLucene library files
AC_MSG_CHECKING([for CLucene library files])
ac_clucene_library_dirs="$ac_clucene_dir/lib $exec_prefix/lib $prefix/lib /usr/lib /usr/lib64 /usr/local/lib"

if test "x$ac_static_clucene" = "xYES"; then
	SEARCH_LIBS="libclucene.a";
else
	SEARCH_LIBS="libclucene.so libclucene.so.0 libclucene.so.0.0.0 libcluene.a";
fi


AC_CACHE_VAL(ac_cv_clucene_libdir, AC_FIND_FILE($SEARCH_LIBS, $ac_clucene_library_dirs, ac_cv_clucene_libdir))

if test "x$ac_cv_clucene_libdir" = "xNO"; then
  AC_MSG_ERROR(CLucene library not found. Try to use configure with --with-clucene-dir=/path/to/clucene);
fi

if test "x$ac_static_clucene" = "xYES"; then
	LIB_CLUCENE="$ac_cv_clucene_libdir/libclucene.a";
else
	LIB_CLUCENE="-lclucene";
fi

AC_SUBST(CLUCENE_LIBRARY_PATH)
AC_SUBST(LIB_CLUCENE)
all_libraries="$all_libraries -L$ac_cv_clucene_libdir"

if test "x$ac_static_clucene" = "xYES"; then
	MESSAGE="static library $ac_cv_clucene_libdir/libclucene.a";
else
	MESSAGE="$ac_cv_clucene_libdir";
fi
AC_MSG_RESULT([$MESSAGE])

dnl -- try to find CLucene include files --
AC_MSG_CHECKING([for Clucene include files])
ac_clucene_include_dirs="$ac_clucene_dir/include $exec_prefix/include $prefix/include /usr/include /usr/local/include"

AC_CACHE_VAL(ac_cv_clucene_incdir, AC_FIND_FILE(CLucene.h, $ac_clucene_include_dirs, ac_cv_clucene_incdir))

if test "x$ac_cv_clucene_incdir" = "xNO"; then
	AC_MSG_ERROR([The CLucene include file files were not found.
Please try to use configure with --with-clucene-dir=/path/to/clucene 
])
fi

CLUCENE_INCLUDES="-I$ac_cv_clucene_incdir"
AC_SUBST(CLUCENE_INCLUDES)
all_includes="$all_includes -I$ac_cv_clucene_incdir -I$ac_cv_clucene_libdir -I$ac_cv_sword_incdir" #$ac_cv_clucene_libdir needed for Clucene/clucene-config.h

AC_MSG_RESULT([$ac_cv_clucene_incdir])



dnl -- CLucene installed test --

AC_MSG_CHECKING([for CLucene version])

AC_CACHE_VAL(ac_cv_installed_clucene_version,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
ac_LD_LIBRARY_PATH_safe=$LD_LIBRARY_PATH
ac_LIBRARY_PATH="$LIBRARY_PATH"
ac_cxxflags_safe="$CXXFLAGS"
ac_ldflags_safe="$LDFLAGS"
ac_libs_safe="$LIBS"

CXXFLAGS="$CXXFLAGS -I$"
LDFLAGS="$LDFLAGS -L$ac_cv_clucene_libdir -L$ac_cv_sword_libdir"
LIBS="$LIB_CLUCENE -lz -lsword"
LD_LIBRARY_PATH="$ac_cv_clucene_libdir"
export LD_LIBRARY_PATH
LIBRARY_PATH=
export LIBRARY_PATH

cat > conftest.$ac_ext <<EOF
#include <iostream>
#include <swversion.h>
#include <CLucene/clucene-config.h>

int main(int argc, char* argv[[]]) {
	if ( sword::SWVersion( _CL_VERSION ) >= sword::SWVersion( "0.9.16" ) ){
		std::cout << "ok";
	}
	else{
		std::cout << "not-ok";
	}
	return 0;
}
EOF


ac_link='$LIBTOOL_SHELL --silent --mode=link ${CXX-g++} -o conftest $CXXFLAGS $all_includes $CPPFLAGS $LDFLAGS conftest.$ac_ext $LIBS 1>&5'
if AC_TRY_EVAL(ac_link) && test -s conftest; then
	if test -x conftest; then
		eval ac_cv_installed_clucene_version=`./conftest 2>&5`
	fi
else
  echo "configure: failed program was:" >&AC_FD_CC
  AC_MSG_ERROR([Failed to compile the test program to check the CLucene version! Please have a look at config.log! Report this to the BibleTime developers!]);
  cat conftest.$ac_ext >&AC_FD_CC
fi

rm -f conftest*
CXXFLAGS="$ac_cxxflags_safe"
LDFLAGS="$ac_ldflags_safe"
LIBS="$ac_libs_safe"

LD_LIBRARY_PATH="$ac_LD_LIBRARY_PATH_safe"
export LD_LIBRARY_PATH
LIBRARY_PATH="$ac_LIBRARY_PATH"
export LIBRARY_PATH
AC_LANG_RESTORE
])

if test "x$ac_cv_installed_clucene_version" = "xok"; then
	AC_MSG_RESULT([ok, version is recent enough]);
elif test "x$ac_cv_installed_clucene_version" = "xnot-ok"; then
	AC_MSG_RESULT([installed]);
  	AC_MSG_ERROR([Your CLucene version is not recent enough! Please upgrade to version >= 0.9.16!]);
fi;

])

