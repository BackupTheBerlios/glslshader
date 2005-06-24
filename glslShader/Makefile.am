SUBDIRS = doc

lib_LTLIBRARIES = libglslShader.la

libglslShader_la_SOURCES = glslShader.c

pkgincludedir = $(includedir)/@PACKAGE@
pkginclude_HEADERS= glslShader.h\
					glslPlatform.h\
					glslTypes.h\
					config.h\
					glext.h

libglslShader_la_LDFLAGS = $(SHARED_FLAGS) -version-info @MAIN_VERSION_INFO@

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = glslShader.pc

