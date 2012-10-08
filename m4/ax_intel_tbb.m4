# SYNOPSIS
#
#   AX_TBB_BASE([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   Test for the Intel TBB C++ libraries of a particular version (or newer)
#
#   If no path to the installed tbb library is given the macro searchs
#   under /usr, /usr/local, /opt and /opt/local
#
#   This macro calls:
#
#     AC_SUBST(TBB_CPPFLAGS) / AC_SUBST(TBB_LDFLAGS)
#
#   And sets:
#
#     HAVE_TBB
#
# Based on the autoconf script for boost, copyright notice below:
#
# COPYLEFT
#
#  Copyright (c) 2008 Thomas Porschberg <thomas@randspringer.de>
#
#  Copying and distribution of this file, with or without modification, are
#  permitted in any medium without royalty provided the copyright notice
#  and this notice are preserved.
#
AC_DEFUN([AX_INTEL_TBB],
[
AC_ARG_WITH([tbb],
	AS_HELP_STRING([--with-tbb@<:@=DIR@:>@], [compile with Intel TBB multithreading support.  Optionally give path to root directory for the header files (optional)]),
	[
    if test "$withval" = "yes"; then
        want_tbb="yes"
        ac_tbb_path=""
    else
	    want_tbb="yes"
        ac_tbb_path="$withval"
	fi
    ],
    [want_tbb="no"])


AC_ARG_WITH([tbb-libdir],
        AS_HELP_STRING([--with-tbb-libdir=LIB_DIR],
        [Force given directory for the Intel TBB libraries. Note that this will overwrite library path detection, so use this parameter only if default library detection fails and you know exactly where your tbb libraries are located.]),
        [
        if test -d $withval
        then
                ac_tbb_lib_path="$withval"
        else
                AC_MSG_ERROR(--with-tbb-libdir expected directory name)
        fi
        ],
        [ac_tbb_lib_path=""]
)

if test "x$want_tbb" = "xyes"; then
	tbb_lib_version_req=ifelse([$1], ,4.0,$1)
	tbb_lib_version_req_shorten=`expr $tbb_lib_version_req : '\([[0-9]]*\.[[0-9]]*\)'`
	tbb_lib_version_req_major=`expr $tbb_lib_version_req : '\([[0-9]]*\)'`
	tbb_lib_version_req_minor=`expr $tbb_lib_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
	WANT_TBB_VERSION=`expr $tbb_lib_version_req_major \* 10 \+  $tbb_lib_version_req_minor`
	AC_MSG_CHECKING(for tbblib >= $tbb_lib_version_req)
	succeeded=no

	dnl first we check the system location for tbb libraries
	dnl this location ist chosen if tbb libraries are installed with the --layout=system option
	dnl or if you install tbb with RPM
	if test "$ac_tbb_path" != ""; then
		TBB_LDFLAGS="-L$ac_tbb_path/lib"
		TBB_CPPFLAGS="-I$ac_tbb_path/include"
	else
		for ac_tbb_path_tmp in /usr /usr/local /opt /opt/local ; do
			if test -d "$ac_tbb_path_tmp/include/tbb" && test -r "$ac_tbb_path_tmp/include/tbb"; then
				TBB_LDFLAGS="-L$ac_tbb_path_tmp/lib"
				TBB_CPPFLAGS="-I$ac_tbb_path_tmp/include"
				break;
			fi
		done
	fi

    dnl overwrite ld flags if we have required special directory with
    dnl --with-tbb-libdir parameter
    if test "$ac_tbb_lib_path" != ""; then
       TBB_LDFLAGS="-L$ac_tbb_lib_path"
    fi

	CPPFLAGS_SAVED="$CPPFLAGS"
	CPPFLAGS="$CPPFLAGS $TBB_CPPFLAGS"
	export CPPFLAGS

	LDFLAGS_SAVED="$LDFLAGS"
	LDFLAGS="$LDFLAGS $TBB_LDFLAGS"
	export LDFLAGS

	AC_LANG_PUSH(C++)
     	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
	@%:@include <tbb/tbb_stddef.h>
	]], [[
	#if TBB_VERSION_MAJOR*10+TBB_VERSION_MINOR >= $WANT_TBB_VERSION
	// Everything is okay
	#else
	#  error Intel TBB version is too old
	#endif
	]])],[
        AC_MSG_RESULT(yes)
	succeeded=yes
	found_system=yes
       	],[
       	])
	AC_LANG_POP([C++])



	dnl if we found no tbb with system layout we search for tbb libraries
	if test "x$succeeded" != "xyes"; then
		_version=0
		if test "$ac_tbb_path" != ""; then
			if test -d "$ac_tbb_path" && test -r "$ac_tbb_path"; then
				for i in `ls -d $ac_tbb_path/include/tbb-* 2>/dev/null`; do
					_version_tmp=`echo $i | sed "s#$ac_tbb_path##" | sed 's/\/include\/tbb-//' | sed 's/_/./'`
					V_CHECK=`expr $_version_tmp \> $_version`
					if test "$V_CHECK" = "1" ; then
						_version=$_version_tmp
					fi
					VERSION_UNDERSCORE=`echo $_version | sed 's/\./_/'`
					TBB_CPPFLAGS="-I$ac_tbb_path/include/tbb-$VERSION_UNDERSCORE"
				done
			fi
		else
			for ac_tbb_path in /usr /usr/local /opt /opt/local ; do
				if test -d "$ac_tbb_path" && test -r "$ac_tbb_path"; then
					for i in `ls -d $ac_tbb_path/include/tbb-* 2>/dev/null`; do
						_version_tmp=`echo $i | sed "s#$ac_tbb_path##" | sed 's/\/include\/tbb-//' | sed 's/_/./'`
						V_CHECK=`expr $_version_tmp \> $_version`
						if test "$V_CHECK" = "1" ; then
							_version=$_version_tmp
	               					best_path=$ac_tbb_path
						fi
					done
				fi
			done

			VERSION_UNDERSCORE=`echo $_version | sed 's/\./_/'`
			TBB_CPPFLAGS="-I$best_path/include/tbb-$VERSION_UNDERSCORE"
            		if test "$ac_tbb_lib_path" = ""
            		then
               			TBB_LDFLAGS="-L$best_path/lib"
            		fi

		fi

		CPPFLAGS="$CPPFLAGS $TBB_CPPFLAGS"
		export CPPFLAGS
		LDFLAGS="$LDFLAGS $TBB_LDFLAGS"
		export LDFLAGS

		AC_LANG_PUSH(C++)
	     	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
		@%:@include <tbb/tbb_stddef.h>
		]], [[
		#if TBB_VERSION_MAJOR*10+TBB_VERSION_MINOR >= $WANT_TBB_VERSION
		// Everything is okay
		#else
		#  error Intel TBB version is too old
		#endif
		]])],[
        	AC_MSG_RESULT(yes)
		succeeded=yes
		found_system=yes
       		],[
	       	])
		AC_LANG_POP([C++])
	fi

	if test "$succeeded" != "yes" ; then
		if test "$_version" = "0" ; then
			AC_MSG_ERROR([[We could not detect the Intel TBB libraries (version $tbb_lib_version_req_shorten or higher). Please give a PATH to the --with-tbb option.  If you are sure you have tbb installed, then check your version number looking in <tbb/tbb_stddef.h>.]])
		else
			AC_MSG_NOTICE([Your Intel TBB library seems to old (version $_version).])
		fi
	else
		TBB_LDFLAGS="-ltbb $TBB_LDFLAGS"
		export TBB_LDFLAGS
		AC_SUBST(TBB_CPPFLAGS)
		AC_SUBST(TBB_LDFLAGS)
		AC_DEFINE(HAVE_TBB,,[define if the Intel TBB library is available])
	fi

        CPPFLAGS="$CPPFLAGS_SAVED"
       	LDFLAGS="$LDFLAGS_SAVED"
fi

])
