

# 
# Synopsis
# 
# AX_CHECK_GL
# 
# Description
# 
# Check for an OpenGL implementation. If GL is found, the required
# compiler and linker flags are included in the output variables
# "GL_CFLAGS" and "GL_LIBS", respectively. This macro adds the configure
# option "--with-apple-opengl-framework", which users can use to
# indicate that Apple's OpenGL framework should be used on Mac OS X. If
# Apple's OpenGL framework is used, the symbol
# "HAVE_APPLE_OPENGL_FRAMEWORK" is defined. If no GL implementation is
# found, "no_gl" is set to "yes".
# 
# Version
# 
# 1.6
# 
# Author
# 
# Braden McDaniel <braden@endoframe.com>
# 
# M4 Source Code
#
AC_DEFUN([AX_CHECK_GL],
[AC_REQUIRE([AC_PATH_X])dnl

#
# There isn't a reliable way to know we should use the Apple OpenGL framework
# without a configure option.  A Mac OS X user may have installed an
# alternative GL implementation (e.g., Mesa), which may or may not depend on X.
#
AC_ARG_WITH([apple-opengl-framework],
            [AC_HELP_STRING([--with-apple-opengl-framework],
                            [use Apple OpenGL framework (Mac OS X only)])])
if test "X$with_apple_opengl_framework" = "Xyes"; then
  AC_DEFINE([HAVE_APPLE_OPENGL_FRAMEWORK], [1],
            [Use the Apple OpenGL framework.])
  GL_LIBS="-framework OpenGL"
else
  GL_CFLAGS="${PTHREAD_CFLAGS}"
  GL_LIBS="${PTHREAD_LIBS} -lm"

  #
  # Use x_includes and x_libraries if they have been set (presumably by
  # AC_PATH_X).
  #
  if test "X$no_x" != "Xyes"; then
    if test -n "$x_includes"; then
      GL_CFLAGS="-I${x_includes} ${GL_CFLAGS}"
    fi
    if test -n "$x_libraries"; then
      GL_LIBS="-L${x_libraries} -lX11 ${GL_LIBS}"
    fi
  fi

  AC_LANG_PUSH(C)

  AC_CHECK_HEADERS([windows.h])

  AC_CACHE_CHECK([for OpenGL library], [ax_cv_check_gl_libgl],
  [ax_cv_check_gl_libgl="no"
  ax_save_CPPFLAGS="${CPPFLAGS}"
  CPPFLAGS="${GL_CFLAGS} ${CPPFLAGS}"
  ax_save_LIBS="${LIBS}"
  LIBS=""
  ax_check_libs="-lopengl32 -lGL"
    for ax_lib in ${ax_check_libs}; do
    if test "X$CC" = "Xcl"; then
      ax_try_lib=`echo $ax_lib | sed -e 's/^-l//' -e 's/$/.lib/'`
    else
      ax_try_lib="${ax_lib}"
    fi
    LIBS="${ax_try_lib} ${GL_LIBS} ${ax_save_LIBS}"
    AC_TRY_LINK([
# if HAVE_WINDOWS_H && defined(_WIN32)
#   include <windows.h>
# endif
# include <GL/gl.h>
],
    [glBegin(0)],
    [ax_cv_check_gl_libgl="${ax_try_lib}"; break])
  done
  LIBS=${ax_save_LIBS}
  CPPFLAGS=${ax_save_CPPFLAGS}])

  if test "X${ax_cv_check_gl_libgl}" = "Xno"; then
    no_gl="yes"
    GL_CFLAGS=""
    GL_LIBS=""
  else
    GL_LIBS="${ax_cv_check_gl_libgl} ${GL_LIBS}"
  fi
  AC_LANG_POP(C)
fi

AC_SUBST([GL_CFLAGS])
AC_SUBST([GL_LIBS])
])dnl




# Check target compiler
AC_DEFUN([SETUP_FOR_TARGET],
[case $target in
*-*-cygwin* | *-*-mingw* | *-*-pw32*)
	AC_SUBST(SHARED_FLAGS, "-shared -no-undefined -Xlinker --export-all-symbols")
	AC_SUBST(PLUGIN_FLAGS, "-shared -no-undefined -avoid-version")
	AC_SUBST(GL_LIBS, "-lopengl32 -lglu32")	
	AC_CHECK_TOOL(RC, windres)
        nt=true
;;
*-*-darwin*)
        AC_SUBST(SHARED_FLAGS, "-shared")
        AC_SUBST(PLUGIN_FLAGS, "-shared -avoid-version")
        AC_SUBST(GL_LIBS, "-lGL -lGLU")
        osx=true
;;
 *) dnl default to standard linux
	AC_SUBST(SHARED_FLAGS, "-shared")
	AC_SUBST(PLUGIN_FLAGS, "-shared -avoid-version")
	AC_SUBST(GL_LIBS, "-lGL -lGLU")
        linux=true
;;
esac
#dnl you must arrange for every AM_conditional to run every time configure runs
#AM_CONDITIONAL(NR_NT, test x$nt = xtrue)
#AM_CONDITIONAL(NR_LINUX, test x$linux = xtrue)
#AM_CONDITIONAL(NR_OSX,test x$osx = xtrue )
])

 

# Check for PIC support
AC_DEFUN([CHECK_PIC],
[
AC_MSG_CHECKING([whether -fPIC is needed])
    case $build in
        x86_64-*)
            CXXFLAGS="$CXXFLAGS -fPIC"
            echo "yes"
        ;;
        *)
            echo "no"
        ;;
    esac
])
