/***************************************************************************
 * GLSL - Utilities                                                        *
 *                                                                         *
 * This library provide simple using of glsl-Language in your              *
 * appliations. Syntax is similar to CG's one.                             *
 *                                                                         *
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

#ifndef _GLSL_UTILS_H_
#define _GLSL_UTILS_H_

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
extern "C"
{
#include "GL/gl.h"
#include "GL/glext.h"
#include "GL/glx.h"
}
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>


#define GLSL_LONGEST_NAME 256
#define GLSL_STACK_SIZE 64



#define GLSL_ERR_NOT_VALID_PROGRAM 	"Not a valid program object!\n"
#define GLSL_ERR_NO_PARAMETERS 		"The program has no parameters or they are not in use!\n"
#define GLSL_ERR_CAN_NOT_CREATE		"Cannot create program object !!!\n Check if OpenGL subsystem is online.\n"
#define GLSL_ERR_WRONG_SHADER_TYPE	"Wrong shader type !\n"
#define GLSL_ERR_CANNOT_OPEN_FILE	"Can not open the shader file \"%s\" \n"
#define GLSL_ERR_NOT_LINKED			"Program object was not linked before!\n"
#define GLSL_ERR_NO_PARAMETER		"The parameter %s not exists or is not used\n"
#define GLSL_ERR_OUT_OF_MEMORY		"Out of Memory Error !!!\n"
#define GLSL_ERR_ONLY_BEFORE_LINK	"This can be used only before program was linked!\n"


// Forward declarations
struct GLSL_Shader;
struct GLSL_Param;

 
/**
 * Error callback function
 **/
typedef GLSL_Shader* 				GLSLprogram;
typedef void(*GLSLerrorCallback)(GLSLprogram, void*);



/**
 * This structure stores information about parameters in glsl-Program
 **/
struct GLSL_Param {
	GLint location;
	GLenum type;
	int length;
	char name[GLSL_LONGEST_NAME];
};



/**
 * Here we store information about attributes used in programs
 **/
struct GLSL_Attr{
	int index;
	char name[GLSL_LONGEST_NAME];
};


/**
 * This represents one shader object. Each shader object is either fragment
 * or vertex program.
 **/
struct GLSL_Shader {

	// OpenGL's program handler
	GLhandleARB program;

	// Here we store parameters for the program
	GLSL_Param* params;
	int param_count;

	// Here we store attributes used in the program
	GLSL_Attr*  attrs;
	int attr_count;
	
	// compiled or not
	bool is_valid;
	
};



//-----------------------------------------------------------------------------------
// General programming functions
//-----------------------------------------------------------------------------------

/**
 * This function is acting like glPushMatrix function. It will push your current
 * program on the stack and will disable it. After the program is on the stack
 * you can use other programs until you pop the program from the stack back.
 *
 * Example:
 * 	glslCreateProgram(...);
 *	glslAttachShader(...);
 *	glslLinkProgram(...);
 *	glslEnableProgram(...);
 *	...
 *	glslPush();		// this wil push current program on the stack, so it can be restored later
 *	glslDisable();// here you disable running glsl program
 *	...						// Do something without shading
 *	glslPop();		// from here the program will continue to run
 *
 * @note This mechanism will only work if you use the glsl* functions. If you
 * enable or disable any program without using of glslEnable(), glslDisable()
 * the behavior of the stack operations is not defined
 **/
void glslPush();
void glslPop();


/**
 * Here we create the program object so it can be filled out
 * with shader programs (more than one is allowed for each kind of shaders).
 * After that you have to compile it, to allow using of it.
 **/
GLSLprogram glslCreateProgram();



/**
 * This will attach any shader to the existent program.
 * You have to call this after you have created the program. The given
 * file will be loaded, compiled and attached to the program.
 * If there was any error, callback will be called and last error string
 * will contain information about the error.
 *
 * @param obj Shader program created before
 * @param filename File name of the shader program
 * @param shaderType Type of the shader program. It can be either:
 *		- GL_VERTEX_SHADER_ARB if file contains vertex program
 *		- GL_FRAGMENT_SHADER_ARB if file contains fragment program
 **/
void 		glslAttachShader(GLSLprogram obj, const char* filename, GLenum shaderType);
void 		glslAttachVertexShader(GLSLprogram obj, const char* filename);
void 		glslAttachFragmentShader(GLSLprogram obj, const char* filename);


/**
 * After you have created the program object and attached shader files to it
 * you can link the program. Linking will help to connect varying variables
 * between vertex and fragment shader. Also it helps to connect used shader
 * libraries function within shader objects.
 *
 * You have to call this function before you can use the GLSL program, otherwise
 * nothing happens.
 *
 * @param obj Shader program created before.
 **/
void 		glslLinkProgram(GLSLprogram obj);


/**
 * Delete the program and release used memory.
 * @param obj Shader program created before
 **/
void 		glslDeleteProgram(GLSLprogram obj);


/**
 * Enable and bind the program for using in the next rendering steps.
 * @param obj SHader program created before.
 **/
void glslEnableProgram(GLSLprogram obj);


/**
 * Disable the programm. So default OpenGL's pipeline will be used by rendering
 * the coming data.
 * @param obj Program to be deisbaled
 **/
void glslDisableProgram(GLSLprogram obj);


/**
 * This function will disable all currently active shader programs
 **/
void glslDisable();



//-----------------------------------------------------------------------------------
// Error Handling
//-----------------------------------------------------------------------------------

/**
 * Returns the last error produced by GLSL compiler or linker.
 * So if your programm does not load, you can find information
 * about the error here.
 **/
const char*	glslGetLastErrorString();


/**
 * Set the callback function which will be called if any error occurs.
 **/
void 		glslSetErrorCallback(GLSLerrorCallback pCallback, void* callbackParam = NULL);


/**
 * Returns 1 if there is an error. 0 otherwise
 **/
int			glslHasError();



//-----------------------------------------------------------------------------------
// Parameter Handling
//-----------------------------------------------------------------------------------

/**
 * Just bind an attribute index to a name of attribute parameter
 * in the glsl program.
 *
 * @param obj Program object created before
 * @param attrIndex index of the attribute used in your application
 * @param name Name of the attribute in the glsl program
 * @note If the program was already linked, so it will be relinked again, because
 *			all bounded attributes has to be linked with the program
 **/
void glslBindAttribute(GLSLprogram obj, int attrIndex, char* name);


/**
 * Returns the number of uniform parameters used in programs.
 *
 * @note If you have any parameters that are not in use, so they
 *		 will be removed by glsl compiler.
 **/
int glslGetUniformParameterCount(GLSLprogram obj);


/**
 * Float version.
 * This function will set new value for the given parameter in glsl program.
 * Parameters should be definded as uniform in your program. Because you
 * can load more than one file for one program it is safer to use different
 * names for parameters.
 *
 * @param obj Shader program created before
 * @param name Parameter name
 * @param size Size of the parameter (vec4 => size=4)
 * @param value Pointer containing vector of data for the parameter
 *
 * @note At now there is only support for values with the length 1,2,3,4
 *		 according to GLSL's float, vec2, vec3, vec4
 **/
void glslSetParameterf(GLSLprogram obj, const char* name, int size, const float* value);


/**
 * Integer version.
 * @see glslSetParameterf() for more information.
 *
 * @note At now there is only support for values with the length 1,2,3,4
 *		 according to GLSL's int, ivec2, ivec3, ivec4
 **/
void glslSetParameteri(GLSLprogram obj, const char* name, int size, const int* value);


/**
 * Same as glslSetParameterf() but here you set matricies as parameters.
 *
 * @param obj Shader program created before
 * @param name Parameter name
 * @param size Size of the parameter (mat4 => size=16)
 * @param value Pointer containing the matrix
 * @param trans If true the matrix will be transformed through OpenGL
 *
 * @note At now there is only support for values with the length 4,9,16
 *		 according to GLSL's mat2, mat3, mat4
 **/
void glslSetMatrixParameter(GLSLprogram obj, const char* name, int size, const float* mat, bool trans = false);


/**
 * Bind the texture to specified parameter name.
 *
 * @param obj Shader program created before
 * @param name Name of the parameter containing texture sampler
 * @param texture_unit OpenGL's texture unit (e.g. for GL_TEXTURE4 texture_unit=4)
 *
 * @note Don't forget to call glBindTexture before you use it. 
 **/
void glslSetTexture(GLSLprogram obj, const char* name, GLuint texture_unit);



#endif

