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

#ifndef _GLSL_SHADER_H_
#define _GLSL_SHADER_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* Platform configuration script */
#include "glslTypes.h"


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
GLSLAPI void GLSLAPIENTRY glslPush();


/**
 * Get the last program from the stack and use it.
 * @see glslPush();
 **/
GLSLAPI void GLSLAPIENTRY glslPop();


/**
 * Here we create the program object so it can be filled out
 * with shader programs (more than one is allowed for each kind of shaders).
 * After that you have to compile it, to allow using of it.
 * @return GLSL program object that will be used later to access to the program
 **/
GLSLAPI GLSLprogram GLSLAPIENTRY glslCreateProgram();



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
 *		- GLSL_VERTEX if file contains vertex program
 *		- GLSL_FRAGMENT if file contains fragment program
 **/
GLSLAPI void GLSLAPIENTRY	glslAttachShader(GLSLprogram obj, const char* filename, GLSL_ShaderType shaderType);


/**
 * Same as glslAttachShader, but attaches a vertex shader program
 * @see glslAttachShader()
 **/
GLSLAPI void GLSLAPIENTRY	glslAttachVertexShader(GLSLprogram obj, const char* filename);

/**
 * Attach a fragment shader program.
 * @see glslAttachShader()
 **/
GLSLAPI void GLSLAPIENTRY	glslAttachFragmentShader(GLSLprogram obj, const char* filename);

/**
 * Same as glslAttachShader, but here you attach shaders from a character array
 * containing the program. This allows you to use your own file loading routine
 * for loading the programs.
 * @param obj Program object created before
 * @param prog String containing the program
 * @param size Size of the given string
 * @param shaderType Type of the shader you want to attach
 * @see glslAttachShader()
 **/
GLSLAPI void GLSLAPIENTRY	glslAttachShaderFromMemory(GLSLprogram obj, const char* prog, size_t size, GLSL_ShaderType shaderType);


/**
 * Same as glslAttachShaderFromMemory, but attaches a vertex shader program
 * @see glslAttachShaderFromMemory()
 **/
GLSLAPI void GLSLAPIENTRY	glslAttachVertexShaderFromMemory(GLSLprogram obj, const char* prog, size_t size);

/**
 * Attach a fragment shader program.
 * @see glslAttachShader()
 **/
GLSLAPI void GLSLAPIENTRY	glslAttachFragmentShaderFromMemory(GLSLprogram obj, const char* prog, size_t size);


/**
 * After you have created the program object and attached shader files to it
 * you should link the program. Linking will help to connect varying variables
 * between vertex and fragment shader. Also it helps to connect used shader
 * libraries function within shader objects.
 *
 * You have to call this function before you can use the GLSL program, otherwise
 * nothing happens.
 *
 * @param obj Shader program created before.
 **/
GLSLAPI void GLSLAPIENTRY	glslLinkProgram(GLSLprogram obj);


/**
 * Delete the program and release used memory.
 * @param obj Shader program created before
 **/
GLSLAPI void GLSLAPIENTRY	glslDeleteProgram(GLSLprogram obj);


/**
 * Enable and bind the program for using in the next rendering steps.
 * @param obj SHader program created before.
 **/
GLSLAPI void GLSLAPIENTRY	glslEnableProgram(GLSLprogram obj);


/**
 * Disable the programm. So default OpenGL's pipeline will be used by rendering
 * the coming data.
 * @param obj Program to be deisbaled
 **/
GLSLAPI void GLSLAPIENTRY	glslDisableProgram(GLSLprogram obj);


/**
 * This function will disable all currently active shader programs
 **/
GLSLAPI void GLSLAPIENTRY	glslDisable();



/*---------------------------------------------------------------------------------*/
/* Error Handling                                                                  */
/*---------------------------------------------------------------------------------*/

/**
 * Returns the last error produced by GLSL compiler or linker.
 * So if your programm does not load, you can find information
 * about the error here.
 **/
GLSLAPI const char* GLSLAPIENTRY	glslGetLastErrorString();


/**
 * Set the callback function which will be called if any error occurs.
 **/
GLSLAPI void GLSLAPIENTRY	glslSetErrorCallback(GLSLerrorCallback pCallback, void* callbackParam);


/**
 * Returns 1 if there is an error. 0 otherwise
 **/
GLSLAPI GLSLbool GLSLAPIENTRY	glslHasError();



/*---------------------------------------------------------------------------------*/
/* Parameter Handling                                                              */
/*---------------------------------------------------------------------------------*/

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
GLSLAPI void GLSLAPIENTRY	glslBindAttribute(GLSLprogram obj, GLSLint32 attrIndex, char* name);


/**
 * Returns the number of uniform parameters used in programs.
 *
 * @note If you have any parameters that are not in use, so they
 *		 will be removed by glsl compiler.
 **/
GLSLAPI GLSLint32 GLSLAPIENTRY	glslGetUniformParameterCount(GLSLprogram obj);


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
GLSLAPI void GLSLAPIENTRY	glslSetParameterf(GLSLprogram obj, const char* name, GLSLint8 size, const GLSLfloat32* value);


/**
 * Same as glslSetParameterf but only for parameters with a length of 1
 * @see glslSetParameterf
 **/
GLSLAPI void GLSLAPIENTRY	glslSetParameter1f(GLSLprogram obj, const char* name, const GLSLfloat32 value){
	glslSetParameterf(obj, name, 1, &value);
}


/**
 * Same as glslSetParameterf but only for parameters with a length of 2
 * @see glslSetParameterf
 **/
GLSLAPI void GLSLAPIENTRY	glslSetParameter2f(GLSLprogram obj, const char* name, const GLSLfloat32* value){
	glslSetParameterf(obj, name, 2, value);
}

/**
 * Same as glslSetParameterf but only for parameters with a length of 3
 * @see glslSetParameterf
 **/
GLSLAPI void GLSLAPIENTRY	glslSetParameter3f(GLSLprogram obj, const char* name, const GLSLfloat32* value){
	glslSetParameterf(obj, name, 3, value);
}

/**
 * Same as glslSetParameterf but only for parameters with a length of 4
 * @see glslSetParameterf
 **/
GLSLAPI void GLSLAPIENTRY	glslSetParameter4f(GLSLprogram obj, const char* name, const GLSLfloat32* value){
	glslSetParameterf(obj, name, 4, value);
}


/**
 * Integer version.
 * @see glslSetParameterf() for more information.
 *
 * @note At now there is only support for values with the length 1,2,3,4
 *		 according to GLSL's int, ivec2, ivec3, ivec4
 **/
GLSLAPI void GLSLAPIENTRY	glslSetParameteri(GLSLprogram obj, const char* name, GLSLint8 size, const GLSLint32* value);


/**
 * Same as glslSetParameteri but only for parameters with a length of 1
 * @see glslSetParameteri
 **/
GLSLAPI void GLSLAPIENTRY	glslSetParameter1i(GLSLprogram obj, const char* name, const GLSLint32 value){
	glslSetParameteri(obj, name, 1, &value);
}


/**
 * Same as glslSetParameteri but only for parameters with a length of 2
 * @see glslSetParameteri
 **/
GLSLAPI void GLSLAPIENTRY	glslSetParameter2i(GLSLprogram obj, const char* name, const GLSLint32* value){
	glslSetParameteri(obj, name, 2, value);
}

/**
 * Same as glslSetParameteri but only for parameters with a length of 3
 * @see glslSetParameteri
 **/
GLSLAPI void GLSLAPIENTRY	glslSetParameter3i(GLSLprogram obj, const char* name, const GLSLint32* value){
	glslSetParameteri(obj, name, 3, value);
}

/**
 * Same as glslSetParameteri but only for parameters with a length of 4
 * @see glslSetParameteri
 **/
GLSLAPI void GLSLAPIENTRY	glslSetParameter4i(GLSLprogram obj, const char* name, const GLSLint32* value){
	glslSetParameteri(obj, name, 4, value);
}



/**
 * Same as glslSetParameterf() but here you set matricies as parameters.
 *
 * @param obj Shader program created before
 * @param name Parameter name
 * @param size Size of the parameter (mat4 => size=16)
 * @param mat Pointer containing the matrix
 * @param trans If 0 the matrix will be not transposed through OpenGL, otherwise yes
 *
 * @note At now there is only support for values with the length 4,9,16
 *		 according to GLSL's mat2, mat3, mat4
 **/
GLSLAPI void GLSLAPIENTRY	glslSetMatrixParameter(GLSLprogram obj, const char* name, GLSLint8 size, const GLSLfloat32* mat, GLSLbool trans);



/** 
 * Same as glslSetMatrixParameter but used for fixed length of the matrix
 * @see glslSetMatrixParameter()
 **/
GLSLAPI void GLSLAPIENTRY	glslSetMatrixParameter4(GLSLprogram obj, const char* name, const GLSLfloat32* mat, GLSLbool trans){
	glslSetMatrixParameter(obj, name, 4, mat, trans);
}

/** 
 * Same as glslSetMatrixParameter but used for fixed length of the matrix
 * @see glslSetMatrixParameter()
 **/
GLSLAPI void GLSLAPIENTRY	glslSetMatrixParameter9(GLSLprogram obj, const char* name, const GLSLfloat32* mat, GLSLbool trans){
	glslSetMatrixParameter(obj, name, 9, mat, trans);
}


/** 
 * Same as glslSetMatrixParameter but used for fixed length of the matrix
 * @see glslSetMatrixParameter()
 **/
GLSLAPI void GLSLAPIENTRY	glslSetMatrixParameter16(GLSLprogram obj, const char* name, const GLSLfloat32* mat, GLSLbool trans){
	glslSetMatrixParameter(obj, name, 16, mat, trans);
}


/**
 * Bind the texture to specified parameter name.
 *
 * @param obj Shader program created before
 * @param name Name of the parameter containing texture sampler
 * @param texture_unit OpenGL's texture unit (e.g. for GL_TEXTURE4 texture_unit=4)
 *
 * @note Don't forget to call glBindTexture() before you use it :-) 
 **/
GLSLAPI void GLSLAPIENTRY	glslSetTexture(GLSLprogram obj, const char* name, GLSLuint32 texture_unit);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

