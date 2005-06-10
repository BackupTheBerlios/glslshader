/***************************************************************************
 *                                                                         *
 *   (c) Art Tevs, MPI Informatik Saarbruecken                             *
 *       mailto: <tevs@mpi-sb.mpg.de>                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 **************************************************************************/
#ifndef __GLSL_PLATFORM_H_
#define __GLSL_PLATFORM_H_

/*
 * Begin system-specific stuff.
 */
#if defined(__BEOS__)
#include <stdlib.h>     /* to get some BeOS-isms */
#endif

#if !defined(OPENSTEP) && (defined(NeXT) || defined(NeXT_PDO))
#define OPENSTEP
#endif

#if defined(_WIN32) && !defined(__WIN32__) && !defined(__CYGWIN__)
#define __WIN32__
#endif

#if !defined(OPENSTEP) && (defined(__WIN32__) && !defined(__CYGWIN__))
#  if defined(_MSC_VER) && defined(BUILD_GLSL32)
#    define GLSLAPI __declspec(dllexport)
#  elif defined(_MSC_VER) && defined(_DLL) /* tag specifying we're building for DLL runtime support */
#    define GLSLAPI __declspec(dllimport)
#  else /* for use with static link lib build of Win32 edition only */
#    define GLSLAPI extern
#  endif /* _STATIC_MESA support */
#  define GLSLAPIENTRY __stdcall
#else
/* non-Windows compilation */
#  define GLSLAPI extern
#  define GLSLAPIENTRY
#endif /* WIN32 / CYGWIN bracket */

/* BeOS */
#if (defined(__BEOS__) && defined(__POWERPC__)) || defined(__QUICKDRAW__)
#  define PRAGMA_EXPORT_SUPPORTED		1
#endif


/* Windows */
#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif


/* Apple Macintosh */
#if defined(macintosh) && PRAGMA_IMPORT_SUPPORTED
#pragma import on
#endif

#if defined(PRAGMA_EXPORT_SUPPORTED)
#pragma export on
#endif

/*
 * End system-specific stuff.
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#endif

