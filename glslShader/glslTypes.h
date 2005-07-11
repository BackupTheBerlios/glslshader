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
 ***************************************************************************/

#ifndef __GLSL_TYPES_H_
#define __GLSL_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*------------------------------------------------------------------------------
	Standard library includings and definitions
------------------------------------------------------------------------------*/
#include "glslPlatform.h"

#define GL_GLEXT_PROTOTYPES
#define GL_GLEXT_LEGACY

#include <GL/gl.h>
#include "glext.h"

/*------------------------------------------------------------------------------
	Basic Type Definitions
------------------------------------------------------------------------------*/
typedef unsigned char 	GLSLbyte;

typedef char			GLSLint8;
typedef unsigned char	GLSLuint8;

typedef signed short	GLSLint16;
typedef	unsigned short	GLSLuint16;

typedef signed int		GLSLint32;
typedef unsigned int	GLSLuint32;

typedef float			GLSLfloat32;
typedef double			GLSLfloat64;

typedef GLSLbyte		GLSLbool;

#ifdef __WIN32__
	typedef   signed __int64  GLSLint64;
	typedef unsigned __int64  GLSLuint64;
#else
	typedef   signed long long GLSLint64;
	typedef unsigned long long GLSLuint64;
#endif


/*------------------------------------------------------------------------------
	Basic program definitions and constants
------------------------------------------------------------------------------*/

//! Size of the longest parameter name that can appears
#define GLSL_LONGEST_NAME 		256

//! How much program should glslPush(), glslPop() support
#define GLSL_STACK_SIZE 		64

//! True value for bool type
#define GLSL_TRUE				0x1

//! False value for bool type
#define GLSL_FALSE				0x0


//! Enumeration for the supported shader types
typedef enum _GLSL_ShaderType
{
	//! The shader is a vertex program
	GLSL_VERTEX,
	
	//! The shader is a fragment program
	GLSL_FRAGMENT
	
} GLSL_ShaderType;



const char* GLSL_ERR_NOT_VALID_PROGRAM 	= "Not a valid program object!\n";
const char* GLSL_ERR_NO_PARAMETERS 		= "The program has no parameters or they are not in use!\n";
const char* GLSL_ERR_CAN_NOT_CREATE		= "Cannot create program object !!!\n Check if OpenGL subsystem is online.\n";
const char* GLSL_ERR_WRONG_SHADER_TYPE	= "Wrong shader type !\n";
const char* GLSL_ERR_CANNOT_OPEN_FILE	= "Can not open the shader file \"%s\" \n";
const char* GLSL_ERR_NOT_LINKED			= "Program object was not linked before!\n";
const char* GLSL_ERR_NO_PARAMETER		= "The parameter %s not exists or is not used\n";
const char* GLSL_ERR_OUT_OF_MEMORY		= "Out of Memory Error !!!\n";
const char* GLSL_ERR_ONLY_BEFORE_LINK	= "This can be used only before program was linked!\n";


//! Here we store information about attributes used in programs
typedef struct _GLSL_Attr
{
	//! Index used to access to the vertex attribute
	int index;
	
	//! Variable name of this attribute used in the program
	char name[GLSL_LONGEST_NAME];
} GLSL_Attr;

//! This structure stores information about parameters in glsl-Program
typedef struct _GLSL_Param 
{
	//! OpenGL's location for this parameter
	GLint location;
	
	//! Type of the parameter (int, vec3, mat4, ...)
	GLenum type;
	
	//! Length of the parameter in units. For example vec3 has the length 3.
	int length;
	
	//! Name of the parameter
	char name[GLSL_LONGEST_NAME];
} GLSL_Param;

//! This represents one shader object. Each shader object is either fragment or vertex program.
typedef struct _GLSL_Shader
{
	//! OpenGL's program handler
	GLhandleARB program;

	//! Here we store parameters for the program
	GLSL_Param* params;
	
	//! Number of parameters in use
	int param_count;

	//! Here we store attributes used in the program
	GLSL_Attr*  attrs;
	
	//! Number of vertex attributes in use
	int attr_count;
	
	//! Is program linked or not
	GLSLbool is_valid;	
} GLSL_Shader;
 

//!This one represents our GLSL Program for using
typedef GLSL_Shader* 				GLSLprogram;

//! Error callback function
typedef void(*GLSLerrorCallback)(GLSLprogram, void*);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
