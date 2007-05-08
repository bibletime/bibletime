##	-*- autoconf -*-
dnl This file was created by Joachim Ansorg <joachim@ansorgs.de>
dnl It provides macord for the autoconf package to find the Sword library on your system.

dnl ----------------------------------------------------------------------
dnl		Check wheter to use static linking
dnl		first parameter is the required version
dnl		second is whether to use static sword library
dnl ----------------------------------------------------------------------
AC_DEFUN(AC_CHECK_SWORD,
[
dnl AC_MSG_CHECKING([for a Sword installation])

dnl The option for the configure script
AC_ARG_WITH(sword-dir,
[  --with-sword-dir=DIR     Path where Sword is being installed (default=/usr) ],
[
  ac_sword_dir=$withval
],ac_sword_dir=/usr
)

AC_ARG_ENABLE(static-sword,
[  --enable-static-sword    Link to the static Sword library],
 	ac_static_sword="YES",
 	[ ac_static_sword="$2" ]
)

dnl try to find Sword library files
AC_MSG_CHECKING([for Sword library files])
AC_REQUIRE([AC_FIND_ZLIB])
ac_sword_library_dirs="$ac_sword_dir/lib $exec_prefix/lib $prefix/lib /usr/lib /usr/lib64 /usr/lib/sword /usr/local/lib /usr/local/lib/sword /usr/local/sword/lib"

if test "x$ac_static_sword" = "xYES"; then
	SEARCH_LIBS="libsword.a";
else
	SEARCH_LIBS="libsword.so libsword.so.3 libsword.so.4 libsword.a";
fi


AC_CACHE_VAL(ac_cv_sword_libdir, AC_FIND_FILE($SEARCH_LIBS, $ac_sword_library_dirs, ac_cv_sword_libdir))

if test "x$ac_cv_sword_libdir" = "xNO"; then
  AC_MSG_ERROR(SWORD library not found. Try to use configure with --with-sword-dir=/your/SWORD/path!);
fi

if test "x$ac_static_sword" = "xYES"; then
	LIB_SWORD="$ac_cv_sword_libdir/libsword.a";
else
	LIB_SWORD="-lsword";
fi

AC_SUBST(SWORD_LIBRARY_PATH)
AC_SUBST(LIB_SWORD)
all_libraries="$all_libraries -L$ac_cv_sword_libdir"

if test "x$ac_static_sword" = "xYES"; then
	MESSAGE="static library $ac_cv_sword_libdir/libsword.a";
else
	MESSAGE="$ac_cv_sword_libdir";
fi
AC_MSG_RESULT([$MESSAGE])

dnl -- try to find Swords include files --
AC_MSG_CHECKING([for Sword include files])
ac_sword_include_dirs="$ac_sword_dir/include/sword $ac_sword_dir/include $exec_prefix/include $prefix/include /usr/include/sword /usr/include /usr/local/include/sword /usr/local/include /usr/local/sword/include /usr/local/sword/include/sword"

AC_CACHE_VAL(ac_cv_sword_incdir, AC_FIND_FILE(swmgr.h, $ac_sword_include_dirs, ac_cv_sword_incdir))

if test "x$ac_cv_sword_incdir" = "xNO"; then
	AC_MSG_ERROR([The Sword include file files were not found.
Please try to use configure with --with-sword-dir=/your/SWORD/path !
])
fi

SWORD_INCLUDES="-I$ac_cv_sword_incdir"
AC_SUBST(SWORD_INCLUDES)
all_includes="$all_includes -I$ac_cv_sword_incdir"

AC_MSG_RESULT([$ac_cv_sword_incdir])



dnl -- Sword version tests --
dnl First test for installed Sword version
dnl Then check if it's recent enough

AC_MSG_CHECKING([for installed Sword version])

AC_CACHE_VAL(ac_cv_installed_sword_version,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
ac_LD_LIBRARY_PATH_safe=$LD_LIBRARY_PATH
ac_LIBRARY_PATH="$LIBRARY_PATH"
ac_cxxflags_safe="$CXXFLAGS"
ac_ldflags_safe="$LDFLAGS"
ac_libs_safe="$LIBS"

CXXFLAGS="$CXXFLAGS -I$"
LDFLAGS="$LDFLAGS -L$ac_cv_sword_libdir"
LIBS="$LIB_SWORD -lz"
LD_LIBRARY_PATH="$ac_cv_sword_libdir"
export LD_LIBRARY_PATH
LIBRARY_PATH=
export LIBRARY_PATH

cat > conftest.$ac_ext <<EOF
#include <iostream>
#include <swversion.h>

#ifdef SWORD_NAMESPACE_START
using namespace sword;
#endif

int main(int argc, char* argv[]) {
	std::cout << SWVersion::currentVersion << std::endl;
	return 0;
}
EOF


ac_link='$LIBTOOL_SHELL --silent --mode=link ${CXX-g++} -o conftest $CXXFLAGS $all_includes $CPPFLAGS $LDFLAGS conftest.$ac_ext $LIBS 1>&5'
if AC_TRY_EVAL(ac_link) && test -s conftest; then
	if test -x conftest; then
		eval ac_cv_installed_sword_version=`./conftest 2>&5`
	fi
else
  echo "configure: failed program was:" >&AC_FD_CC
  AC_MSG_ERROR([Failed to compile the test program to check the Sword version! Please have a look at config.log! Report this to the BibleTime developers!]);
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
AC_MSG_RESULT([$ac_cv_installed_sword_version])




dnl *** Now check if the installed version is recent enough
AC_MSG_CHECKING([whether your Sword installation is recent enough])

AC_CACHE_VAL(ac_cv_sword_recent_version,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
ac_LD_LIBRARY_PATH_safe=$LD_LIBRARY_PATH
ac_LIBRARY_PATH="$LIBRARY_PATH"
ac_cxxflags_safe="$CXXFLAGS"
ac_ldflags_safe="$LDFLAGS"
ac_libs_safe="$LIBS"

CXXFLAGS="$CXXFLAGS -I$"
LDFLAGS="$LDFLAGS -L$ac_cv_sword_libdir"
LIBS="$LIB_SWORD -lz"
LD_LIBRARY_PATH="$ac_cv_sword_libdir"
export LD_LIBRARY_PATH
LIBRARY_PATH=
export LIBRARY_PATH

cat > conftest.$ac_ext <<EOF
#include <iostream>
#include <swversion.h>

#ifdef SWORD_NAMESPACE_START
using namespace sword;
#endif


int main(int argc, char* argv[[]]) {
	if (argc == 2) { //compare required with installed Sword version
		const char* requiredVersion( argv[[1]] );
		if ( (SWVersion( requiredVersion ) < SWVersion::currentVersion) || (SWVersion( requiredVersion ) == SWVersion::currentVersion) ) {
			std::cout << "ok";
			return -1;
		}
		else {
			std::cout << "not-ok";
			return -2; //version not recent enough
		}
	}
	return 0;
}
EOF

ac_link='$LIBTOOL_SHELL --silent --mode=link ${CXX-g++} -o conftest $CXXFLAGS $all_includes $CPPFLAGS $LDFLAGS conftest.$ac_ext $LIBS 1>&5'
if AC_TRY_EVAL(ac_link) && test -s conftest; then
	if test -x conftest; then
		eval ac_cv_sword_recent_version=`./conftest $1 2>&5`
	fi
else
  echo "configure: failed program was:" >&AC_FD_CC
  AC_MSG_ERROR([Failed to compile the test program to check the Sword version! Please have a look at config.log! Report this to the BibleTime developers!]);
  cat conftest.$ac_ext >&AC_FD_CC
fi;

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

if test "x$ac_cv_sword_recent_version" = "xok"; then
	AC_MSG_RESULT([yes]);
elif test "x$ac_cv_sword_recent_version" = "xnot-ok"; then
	AC_MSG_RESULT([no]);
	AC_MSG_ERROR([Your Sword installation is not recent enought! Please upgrade to version $1! Get the Sword library at www.crosswire.org.]);
else
	AC_MSG_ERROR([Invalid return code of the internal Sword version test program. Please submit a bugreport!]);
fi;

])
