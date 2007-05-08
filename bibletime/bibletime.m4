##	-*- autoconf -*-

dnl -------------------------------------------------------------------------------
dnl		Check wheter to use static linking, first parameter is the result (YES/NO)
dnl -------------------------------------------------------------------------------
AC_DEFUN(AC_CHECK_STATIC_LINKING,
[
$1="NO"

AC_MSG_CHECKING(whether to use static linking)
AC_ARG_ENABLE(static-linking,
      [  --enable-static-linking	use static linking],
      USE_STATIC_LINKING=$enableval, USE_STATIC_LINKING="no")

if test "$USE_STATIC_LINKING" = "yes"; then
	BT_LDFLAGS="-all-static -Wl,-Bstatic";
	AC_SUBST(BT_LDFLAGS)
	CPPFLAGS="-DSTATIC_BUILD $CPPFLAGS";
	$1="YES";
else
	$1="NO";
fi

AC_MSG_RESULT($USE_STATIC_LINKING)
])

dnl -------------------------------------------------------------------------------
dnl		Checks the type of the distribution
dnl		Possible names for the distributions are SuSE, RedHat, Mandrake
dnl -------------------------------------------------------------------------------
AC_DEFUN(AC_CHECK_DISTRIBUTION,
[
AC_MSG_CHECKING([for the used Linux distribution])

AC_ARG_WITH(distribution,
[  --with-distribution=TYPE     The name of your *NIX distribution, e.g. SuSE [auto] ],
[
  ac_distribution=$withval
],ac_distribution=auto
)

AC_ARG_WITH(distribution-version,
[  --with-distribution-version=VERSION     Version of your *NIX distribution, e.g. 7.2 [auto] ],
[
  ac_distribution_version=$withval
],ac_distribution_version=auto
)

AC_ARG_WITH(rpm-prefix,
[  --with-rpm-prefix=DIR    The directory which should be used as PREFIX
														in the RPM spec file [auto] ],
[
  ac_rpm_prefix=$withval
],ac_rpm_prefix=auto
)

AC_ARG_WITH(rpm-group,
[  --with-rpm-group=DIR     The group which will be used for the RPM system [auto] ],
[
  ac_rpm_group=$withval
],ac_rpm_group=auto
)

if test "$ac_distribution" != "auto"; then
	DISTRIBUTION=[$ac_distribution]
fi

if test "$ac_distribution" = "auto"; then
AC_CACHE_VAL(DISTRIBUTION, [
if test -f "/etc/SuSE-release"; then
	DISTRIBUTION="SuSE"
elif test -f "/etc/mandrake-release"; then
	DISTRIBUTION="Mandrake"
elif test -f "/etc/redhat-release"; then
	DISTRIBUTION="Red Hat"
elif test -f "/etc/debian_version"; then
	DISTRIBUTION="Debian"
fi
])
fi

if test "$ac_distribution_version" != "auto"; then
	DISTRIBUTION_VERSION=[$ac_distribution_version]
fi
if test "$ac_distribution_version" = "auto"; then
AC_CACHE_VAL(DISTRIBUTION_VERSION, [
if test -f "/etc/SuSE-release" && test "$DISTRIBUTION" = "SuSE"; then
	DISTRIBUTION_VERSION=[`grep "VERSION =" /etc/SuSE-release | sed 's/VERSION = //'`];	
elif test -f "/etc/mandrake-release" && test "$DISTRIBUTION" = "Mandrake"; then
	DISTRIBUTION_VERSION=[`grep "Mandrake" /etc/mandrake-release | sed 's/[a-zA-Z()]*//g' | awk '{print $1}'`]
elif test -f "/etc/redhat-release" && test "$DISTRIBUTION" = "Red Hat"; then
	DISTRIBUTION_VERSION=[`grep "Red Hat" /etc/redhat-release | sed 's/[a-zA-Z()]*//g' | awk '{print $1}'`]
elif test -f "/etc/debian_version" && test "$DISTRIBUTION" = "Debian"; then
	DISTRIBUTION_VERSION=[`cat /etc/debian_version`]
fi
])
fi


if test "$ac_rpm_prefix" != "auto"; then
	RPM_PREFIX=[$ac_rpm_prefix]
fi
if test "$ac_rpm_prefix" = "auto"; then
AC_CACHE_VAL(RPM_PREFIX, [
if test "$DISTRIBUTION" = "SuSE"; then
	RPM_PREFIX="/opt/kde3"
elif test "$DISTRIBUTION" = "Mandrake"; then
	RPM_PREFIX="/usr"	
elif test "$DISTRIBUTION" = "Red Hat"; then
	RPM_PREFIX="/usr"		
elif test "$DISTRIBUTION" = "Debian"; then
	RPM_PREFIX="/usr"
fi
])
fi

if test "$ac_rpm_group" != "auto"; then
	RPM_GROUP=[$ac_rpm_group]
fi
if test "$ac_rpm_prefix" = "auto"; then
AC_CACHE_VAL(RPM_GROUP,
[
if test "$DISTRIBUTION" = "SuSE"; then
	RPM_GROUP="X11/KDE/Applications"
elif test "$DISTRIBUTION" = "Mandrake"; then
	RPM_GROUP="Graphical desktop/KDE"
elif test "$DISTRIBUTION" = "Red Hat"; then
	RPM_GROUP="Graphical desktop/KDE"
elif test "$DISTRIBUTION" = "Debian"; then
	RPM_GROUP="Graphical desktop/KDE"
fi
])
fi

if test -n "$DISTRIBUTION" && test "$DISTRIBUTION" != "unknown"; then
	AC_DEFINE_UNQUOTED(DISTRIBUTION, ${DISTRIBUTION}, [The type of your distribution])
fi

if test -n "$DISTRIBUTION_VERSION"  && test "$DISTRIBUTION_VERSION" != "unknown"; then
	AC_DEFINE_UNQUOTED(DISTRIBUTION_VERSION, ${DISTRIBUTION_VERSION}, [The version of your distribution])
fi

AC_SUBST(DISTRIBUTION)
AC_SUBST(DISTRIBUTION_VERSION)
AC_SUBST(RPM_PREFIX)
AC_SUBST(RPM_GROUP)

AC_MSG_RESULT([$DISTRIBUTION $DISTRIBUTION_VERSION])

AC_MSG_CHECKING([for RPM installation prefix])
AC_MSG_RESULT([$RPM_PREFIX])

AC_MSG_CHECKING([for the correct RPM group])
AC_MSG_RESULT([$RPM_GROUP])
])




dnl MY_CURL
dnl -------
dnl set my_cv_curl_vers to the version of libcurl or NONE
dnl if libcurl is not found or is too old
dnl This function comes from cURL's webpage

AC_DEFUN(AC_CHECK_CURL,[
 AC_CACHE_VAL(my_cv_curl_vers,[
 my_cv_curl_vers=NONE
 dnl check is the plain-text version of the required version
 check="7.9"
 dnl check_hex must be UPPERCASE if any hex letters are present
 check_hex="070900"

 AC_MSG_CHECKING([for curl >= $check])

 if eval curl-config --version 2>/dev/null >/dev/null; then
   ver=`curl-config --version | sed -e "s/libcurl //g"`
   hex_ver=`curl-config --vernum | tr 'a-f' 'A-F'`
   ok=`echo "ibase=16; if($hex_ver>=$check_hex) $hex_ver else 0" | bc`

   if test x$ok != x0; then
     my_cv_curl_vers="$ver"
     AC_MSG_RESULT([$my_cv_curl_vers])
   else
     AC_MSG_RESULT(FAILED)
		 AC_MSG_ERROR([$ver is too old. Please install libcurl version $check or higher.]);
   fi
 else
   AC_MSG_RESULT(FAILED)
   AC_MSG_ERROR([curl-config was not found. Please install libcurl version $check or higher.])
 fi
 ])
])
