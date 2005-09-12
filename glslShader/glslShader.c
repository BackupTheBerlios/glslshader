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

/* Shader header */
#include "glslShader.h"

/* OpenGL's includes */
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#include <GL/glext.h>
#endif

/* Standard library includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>


/* Constants defined for using */
const char* GLSL_ERR_NOT_VALID_PROGRAM 	= "Not a valid program object!\n";
const char* GLSL_ERR_NO_PARAMETERS 		= "The program has no parameters or they are not in use!\n";
const char* GLSL_ERR_CAN_NOT_CREATE		= "Cannot create program object !!!\n Check if OpenGL subsystem is online.\n";
const char* GLSL_ERR_WRONG_SHADER_TYPE	= "Wrong shader type !\n";
const char* GLSL_ERR_CANNOT_OPEN_FILE	= "Can not open the shader file \"%s\" \n";
const char* GLSL_ERR_NOT_LINKED			= "Program object was not linked before!\n";
const char* GLSL_ERR_NO_PARAMETER		= "The parameter %s not exists or is not used\n";
const char* GLSL_ERR_OUT_OF_MEMORY		= "Out of Memory Error !!!\n";
const char* GLSL_ERR_ONLY_BEFORE_LINK	= "This can be used only before program was linked!\n";

#if 0
#include <GL/glut.h>

void error_callback(GLSLprogram obj, void* param){

	const char* LastError = glslGetLastErrorString();
	
	if(LastError){
		printf("\n---------------------------------------------------\n");
		printf("%s\n", LastError);
		printf("---------------------------------------------------\n");
		printf("GLSL error, exiting...\n");
		exit(0);
	}

}

int main(int argc, char* argv[]){

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(100, 100);
	glutCreateWindow("GLSl Tester");

	
	GLSLprogram shader;

	glslSetErrorCallback(error_callback, 0);
	
	shader = glslCreateProgram();
	glslAttachShader(shader, "glsl/vDepthPNG.gl", GL_VERTEX_SHADER_ARB);
	glslLinkProgram(shader);

	
	return 0;
	
}
#endif


/* Internal used functions */
void glslEmptyLastError();
void glslFillLastError(const char* fmt, ...);
void glslCheckError(GLSLprogram obj, GLSLbool internal);
void glslGetParam(GLSLprogram obj, const char* name, GLSL_Param* param);
char* glslScanAttributes(GLSLprogram obj, char* prog, GLSL_Attr **attr, GLSLint32* count);


/**
 * Store here the last error string
 **/
char	___glsl_last_error_string[4096];

/**
 * Store 1 if there is an error stored in variable or 0 otherwise
 **/
GLSLbool		___glsl_has_last_error = GLSL_FALSE;

/**
 * Store here the pointer to specified callback function.
 **/
GLSLerrorCallback ___glsl_error_callback = 0;


/**
 * Store here the specified callback parameter which will be given
 * to the callback fucntion.
 **/
void*			  ___glsl_error_callback_param = 0;


/**
 * This variables do pushing and poping of glsl programs on the stack
 **/
GLhandleARB	_glsl_Program_Stack[GLSL_STACK_SIZE];
GLSLint16	_glsl_Program_StackPos = 0;


/*--------------------------------------------------------------------------------*/
void glslPush(){
	if (_glsl_Program_StackPos > GLSL_STACK_SIZE - 1){
		_glsl_Program_StackPos = GLSL_STACK_SIZE - 1;
	}
	_glsl_Program_Stack[_glsl_Program_StackPos] = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
	_glsl_Program_StackPos++;
}
/*--------------------------------------------------------------------------------*/
void glslPop(){
	if (_glsl_Program_StackPos < 1){
		return;
	}
	_glsl_Program_StackPos--;
	glUseProgramObjectARB(_glsl_Program_Stack[_glsl_Program_StackPos]);
}
/*--------------------------------------------------------------------------------*/
const char*	glslGetLastErrorString(){
	return ___glsl_last_error_string;
}


/**
 * Find parameter in a program by his name.
 * Returns parameter information to the pointer showing at existent parameter
 * structure.
 *
 * @param obj Program object created before
 * @param name Name of the parameter
 * @param param Pointer to parameter.
 **/
void glslGetParam(GLSLprogram obj, const char* name, GLSL_Param* param){

	if (obj == 0) return;

	/* is program valid */
	if (obj->program == 0 || obj->is_valid == 0){
		glslFillLastError(GLSL_ERR_NOT_VALID_PROGRAM);
		glslCheckError(obj, 1);
		return;		
	}

	/* has the program got any parameters */
	if (obj->params == 0){
		glslFillLastError(GLSL_ERR_NO_PARAMETERS);
		glslCheckError(obj, 1);
		return;		
	}
	
	param->location = glGetUniformLocationARB(obj->program, (const GLcharARB*)name);

}

/*--------------------------------------------------------------------------------*/
GLSLprogram glslCreateProgram(){

	GLSLprogram prog = 0;

	prog = (GLSLprogram)malloc(sizeof(GLSL_Shader));
	if (prog == 0) return 0;
	
	/* it is not valid at now */
	prog->is_valid = 0;

	/* setup all parameters */
	prog->params = 0;
	prog->param_count = 0;
	
	/* Create program through OpenGL's context */
	prog->program = glCreateProgramObjectARB();

	if (prog->program == 0){
		glslFillLastError(GLSL_ERR_CAN_NOT_CREATE);
		glslCheckError(prog, 1);
	}else{
		glslEmptyLastError();
	}
	
	return prog;
	
}

/*--------------------------------------------------------------------------------*/
void glslAttachShaderFromMemory(GLSLprogram obj, const char* prog, size_t size, GLSL_ShaderType shaderType)
{
	if (obj == 0) return;
	
	/* check for right shader type */
	if (shaderType != GLSL_VERTEX && shaderType != GLSL_FRAGMENT){
		glslFillLastError(GLSL_ERR_WRONG_SHADER_TYPE);
		glslCheckError(obj, 1);
		return;		
	}

	if (obj->program == 0){
		glslFillLastError(GLSL_ERR_NOT_VALID_PROGRAM);
		glslCheckError(obj, 1);
		return;		
	}

#if 0
	/* read all attributes from the program */
	GLSL_Attr* attr = 0;
	int count = 0;
	char* ndata = glslScanAttributes(obj, prog, &attr, &count);
#endif
	
	/* compile the source */
	GLenum shType = shaderType == GLSL_VERTEX ? GL_VERTEX_SHADER_ARB : GL_FRAGMENT_SHADER_ARB;
	
	//GLint length = size;
	GLhandleARB shader = glCreateShaderObjectARB(shType);
	glShaderSourceARB(shader, 1, (const GLcharARB**)&prog, (const GLint*)&size);
	glCompileShaderARB(shader);
	glAttachObjectARB(obj->program, shader);
	glDeleteObjectARB(shader);

#if 0
	/* Now combine all used attributes and copy them to program object */
	for (int i=0; i < count; i++){
		glBindAttribLocationARB(obj->program, attr[i].index, attr[i].name);
	}
	free(ndata);
#endif
		
	glslCheckError(obj, 0);

}

/*--------------------------------------------------------------------------------*/
void glslAttachShader(GLSLprogram obj, const char* filename, GLSL_ShaderType shaderType){

	if (obj == 0) return;

	char*	data = 0;
	size_t 	size = 0;
	FILE* 	file = 0;

	/* check for right shader type */
	if (shaderType != GLSL_VERTEX && shaderType != GLSL_FRAGMENT){
		glslFillLastError(GLSL_ERR_WRONG_SHADER_TYPE);
		glslCheckError(obj, 1);
		return;		
	}

	if (obj->program == 0){
		glslFillLastError(GLSL_ERR_NOT_VALID_PROGRAM);
		glslCheckError(obj, 1);
		return;		
	}

	
	/* open the file */
	file = fopen(filename,"rb");
	if(!file) {
		glslFillLastError(GLSL_ERR_CANNOT_OPEN_FILE, filename);
		glslCheckError(obj, 1);
		return;
	}

	/* get the size of the file and read it */
	fseek(file,0,SEEK_END);
	size = ftell(file);
	data = (char*)malloc(sizeof(char)*(size + 1));
	memset(data, 0, sizeof(char)*(size + 1));
	fseek(file,0,SEEK_SET);
	fread(data,1,size,file);
	fclose(file);

	/* Attach the shader */
	glslAttachShaderFromMemory(obj, data, size, shaderType);
	
	free(data);

}

/*--------------------------------------------------------------------------------*/
void glslAttachVertexShader(GLSLprogram obj, const char* filename)
{
	glslAttachShader(obj, filename, GLSL_VERTEX);
}

/*--------------------------------------------------------------------------------*/
void glslAttachFragmentShader(GLSLprogram obj, const char* filename)
{
	glslAttachShader(obj, filename, GLSL_FRAGMENT);
}

/*--------------------------------------------------------------------------------*/
void glslAttachVertexShaderFromMemory(GLSLprogram obj, const char* prog, size_t size)
{
	glslAttachShaderFromMemory(obj, prog, size, GLSL_VERTEX);
}

/*--------------------------------------------------------------------------------*/
void glslAttachFragmentShaderFromMemory(GLSLprogram obj, const char* prog, size_t size)
{
	glslAttachShaderFromMemory(obj, prog, size, GLSL_FRAGMENT);
}


/*--------------------------------------------------------------------------------*/
void glslBindAttribute(GLSLprogram obj, GLSLint32 attrIndex, char* name){

	if (obj == 0)return;
	
	glBindAttribLocationARB(obj->program, attrIndex, name);

	/* if already linked, so link again */
	if (obj->is_valid){
		glslLinkProgram(obj);		
	}
}


/**
 * This function will scan the given source code of a glsl prgram
 * for used aatributes. All attributes must be defined with an attribute
 * index which will be used in your application. For example:
 * You want to pass vec4 as vertex attribute to the glsl program. You
 * also have to define the index for that attribute to use in your
 * application. So write in glsl Program:
 *	attribute vec4 a_data : 12;
 * This means that you define attribute with the name a_data and this can
 * be accessed through index number 12.
 *
 * @param obj Program object (will only be used to generate error messages)
 * @param prog Source code of the glsl program.
 * @param attr Here this function will return an array containing all
 *				attributes that were found
 * @param count Here it returns the number of elements in the array
 * @return Here this function will return the given source code back, but
 * now it is not containing any information of used indices, so it can now be
 * compiled through OpenGL
 *
 * @note You have to delete all returned pointer to prevent memory leaks. Also
 * the given string containing the program will not be deleted. You have also
 * to do it by yourself.
 **/
char* glslScanAttributes(GLSLprogram obj, char* prog, GLSL_Attr **attr, GLSLint32* count){

	if (attr == 0 || count == 0){
		return prog;
	}

	/* define the buffer which will hold our code to be returned */
	char* new_prog = 0;
	new_prog = (char*)malloc(strlen(prog)*sizeof(char));
	if (new_prog == 0){
		glslFillLastError(GLSL_ERR_OUT_OF_MEMORY);
		glslCheckError(obj, 1);
		return prog;
	}
	memset(new_prog, 0, sizeof(char)*strlen(prog));
	
	/* temporary attributes storing */
	GLSL_Attr attrTemp[64];
	int attrCount = 0;
		
	/* scan the program */
	char* source = prog;
	char* dest = new_prog;
	while (1){
		/* scan the code for attribute line */
		char* at = strstr(source, "attribute ");

		/* if nothing found, so break the loop */
		if (at == 0){
			static int i = 0;
			i++;
			fprintf(stderr, "%i - %i\n", i, source - prog);
			/* copy now the rest to the new source */
			strcpy(dest, source);
			break;
		}

		/* now at has got right value, so scan now for the index number */
		int index = -1;
		int space_count = 0;
		char name[GLSL_LONGEST_NAME+1];
		int name_index = 0;
		memset(name, 0, sizeof(char) * (GLSL_LONGEST_NAME+1));
		char* s = at;
		while (*s != ';' && *s != '\0'){
			/* ok we found the start of index number */
			if (*s == ':'){
				s++;
				while (*s == ' ' && *s != '\0')s++;				
				if (*s == '\0')break;
				
				/* now the next four characters must be ATTR */
				if (*(s++) != 'A') break;
				if (*(s++) != 'T') break;
				if (*(s++) != 'T') break;
				if (*(s++) != 'R') break;

				/* so if we here so we found the ATTR string */
				/* now scan for the number */
				char buffer[5];
				memset(buffer, 0, 5*sizeof(char));
				int cb = 0;
				while (*s != '\0' && *s != ' ' && *s != ';' && cb < 5){
					buffer[cb] = *s;
					s++;
					cb++;
				}

				/* No number so error */
				if (strlen(buffer) == 0) break;

				/* get the scanned number */
				index = atoi(buffer);

				
			/* we do not see any index number */
			}else{
				
				/* now scan for the name of attribute
				  the name will be found after the second space character */
				if (*s == ' ' || *s == ';' || *s == ':')space_count++;
				if (space_count >= 2){
					/* the next string will be the name */
					if (space_count == 3 && *s != ' ' && *s != ':' && *s != ';'){
						name[name_index++] = *s;
					}
				}
			}
			
			/* copy the source to destination */
			*(dest++) = *s;

			/* next character */
			if (*s != ';') s++;
			
		}

		/* from here we know the name of the attribute and if the
		   index was given also the index number */

		/* if we have no name, so this is an error */
		if (name_index == 0){
			strcpy(dest, source);
			break;
		}

		/* if we have index and the name, so copy it to temporary array */
		attrTemp[attrCount].index = index;
		strcpy(attrTemp[attrCount].name, name);
		attrCount++;

		/* know set the source pointer to right position */
		/* if the source code was correct */
		source = s;
		
	}

	/* now copy all found attributes */
	if (attrCount > 0){
		*attr = (GLSL_Attr*)malloc(sizeof(GLSL_Attr) * attrCount);
		if (*attr != 0){
			*count = attrCount;
			int i = 0;
			for (i=0; i < attrCount; i++){
				((*attr)[i]).index = attrTemp[i].index;
				strcpy(((*attr)[i]).name, attrTemp[i].name);
			}
		}
	}
	
	return new_prog;
}


/*--------------------------------------------------------------------------------*/
void glslLinkProgram(GLSLprogram obj){

	if (obj == 0) return;
	
	/* check if program is valid */
	if (obj->program == 0){
		glslFillLastError(GLSL_ERR_NOT_VALID_PROGRAM);
		glslCheckError(obj, 1);
		return;		
	}

	/* Link the program */
	glLinkProgramARB(obj->program);
	GLint linked;
	glGetObjectParameterivARB(obj->program, GL_OBJECT_LINK_STATUS_ARB, &linked);
	if(!linked) {
		glslCheckError(obj, 0);
		return;
	}

	obj->is_valid = 1;
	glslEmptyLastError();


	/* Get parameters count */
	GLint count = 0;
	glGetObjectParameterivARB(obj->program, GL_OBJECT_ACTIVE_UNIFORMS_ARB, &count);
	
	/* get all uniform parameters in the program */
	if (count > 0){
		obj->param_count = count;
		obj->params = (GLSL_Param*)malloc(count* sizeof(GLSL_Param));
		memset(obj->params, 0, count* sizeof(GLSL_Param));
		
		GLuint i = 0;	
		for (i=0; i < (GLuint)count; i++){
			GLsizei length = 0;
			GLint size = 0;
			GLenum type;
			GLcharARB name[GLSL_LONGEST_NAME];
			memset(name, 0, GLSL_LONGEST_NAME*sizeof(GLcharARB));
			
			glGetActiveUniformARB(obj->program,i, GLSL_LONGEST_NAME, &length, &size, &type, &(name[0]));

			/* Copy name for the uniform parameter */
			if (length < GLSL_LONGEST_NAME)
				memcpy(obj->params[i].name, name, sizeof(char)*length);
			else
				memcpy(obj->params[i].name, name, sizeof(char)*GLSL_LONGEST_NAME);

			/* copy other data */
			obj->params[i].length = size;
			obj->params[i].type = type;
			obj->params[i].location = glGetUniformLocationARB(obj->program, name);
		}
	}
	
}


/*--------------------------------------------------------------------------------*/
void glslDeleteProgram(GLSLprogram obj){
	if (obj == 0) return;
	if (obj->program) glDeleteObjectARB(obj->program);

	if (obj->params != 0)free(obj->params);
	free(obj);
}


/**
 * Empty the last error string.
 **/
void glslEmptyLastError(){
	memset(___glsl_last_error_string, 0, sizeof(___glsl_last_error_string));
	___glsl_has_last_error = 0;
}


/**
 * Fill the last error string.
 **/
void glslFillLastError(const char* fmt, ...){

	char buf[4096];

	va_list(ap);
	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);
	
	strcpy(___glsl_last_error_string, buf);

	___glsl_has_last_error = 1;
}

/**
 * Check for errors. If no error here, so empty the last error string.
 * If there is an error, so copy GLSL's error string to that buffer.
 * Also if there was an error, so call error callback function.
 *
 * We distinguish between internal and external errors. Internal errors
 * are errors produced internal by this program (for example "Wrong parameter
 * name" or something else). External errors are produced by GLSL
 * and will be tested here.
 **/
void glslCheckError(GLSLprogram obj, GLSLbool internal){
	
	GLSLbool occurs = internal;
	
	if (internal == 0){
		size_t length = 0;
		static char error[4096];
		glGetInfoLogARB(obj->program, 4096, (GLsizei*)&length, error);

		glslEmptyLastError();

		if (length > 0){
			strcpy(___glsl_last_error_string, error);
			___glsl_has_last_error = 1;

			if (strstr(error, "warning:") == 0){
				occurs = 1;
			}
		}		
	}
	
	if (___glsl_error_callback && occurs){
		___glsl_error_callback(obj, ___glsl_error_callback_param);
	}

	
}


/*--------------------------------------------------------------------------------*/
GLSLbool glslHasError(){
	return ___glsl_has_last_error;
}


/*--------------------------------------------------------------------------------*/
void glslSetErrorCallback(GLSLerrorCallback pCallback, void* callbackParam){
	___glsl_error_callback = pCallback;
	___glsl_error_callback_param = callbackParam;
}


/*--------------------------------------------------------------------------------*/
void glslSetParameterf(GLSLprogram obj, const char* name, GLSLint8 size, const GLSLfloat32* value){

	if (obj == 0) return;
	if (obj->is_valid == 0){
		glslFillLastError(GLSL_ERR_NOT_LINKED);
		glslCheckError(obj, 1);
		return;		
	}
	
	GLSL_Param param;
	glslGetParam(obj, name, &param);

	if (param.location != -1){
		if(size == 1) glUniform1fvARB(param.location,1,value);
		else if(size == 2) glUniform2fvARB(param.location,1,value);
		else if(size == 3) glUniform3fvARB(param.location,1,value);
		else if(size == 4) glUniform4fvARB(param.location,1,value);
		glslCheckError(obj, 0);
	}else{
		glslFillLastError(GLSL_ERR_NO_PARAMETER, name);
		glslCheckError(obj, 1);
	}
	
}

/*--------------------------------------------------------------------------------*/
void glslSetParameter1f(GLSLprogram obj, const char* name, const GLSLfloat32 value){
	glslSetParameterf(obj, name, 1, &value);
}


/*--------------------------------------------------------------------------------*/
void glslSetParameter2f(GLSLprogram obj, const char* name, const GLSLfloat32* value){
	glslSetParameterf(obj, name, 2, value);
}

/*--------------------------------------------------------------------------------*/
void glslSetParameter3f(GLSLprogram obj, const char* name, const GLSLfloat32* value){
	glslSetParameterf(obj, name, 3, value);
}

/*--------------------------------------------------------------------------------*/
void glslSetParameter4f(GLSLprogram obj, const char* name, const GLSLfloat32* value){
	glslSetParameterf(obj, name, 4, value);
}

/*--------------------------------------------------------------------------------*/
void glslSetParameteri(GLSLprogram obj, const char* name, GLSLint8 size, const GLSLint32* value){

	if (obj == 0) return;
	if (obj->is_valid == 0){
		glslFillLastError(GLSL_ERR_NOT_LINKED);
		glslCheckError(obj, 1);
		return;		
	}
	
	GLSL_Param param;
	glslGetParam(obj, name, &param);

	if (param.location != -1){
		if(size == 1) glUniform1ivARB(param.location,1,value);
		else if(size == 2) glUniform2ivARB(param.location,1,value);
		else if(size == 3) glUniform3ivARB(param.location,1,value);
		else if(size == 4) glUniform4ivARB(param.location,1,value);
		glslCheckError(obj, 0);
	}else{
		glslFillLastError(GLSL_ERR_NO_PARAMETER, name);
		glslCheckError(obj, 1);
	}
}

/*--------------------------------------------------------------------------------*/
void glslSetParameter1i(GLSLprogram obj, const char* name, const GLSLint32 value){
	glslSetParameteri(obj, name, 1, &value);
}


/*--------------------------------------------------------------------------------*/
void glslSetParameter2i(GLSLprogram obj, const char* name, const GLSLint32* value){
	glslSetParameteri(obj, name, 2, value);
}

/*--------------------------------------------------------------------------------*/
void glslSetParameter3i(GLSLprogram obj, const char* name, const GLSLint32* value){
	glslSetParameteri(obj, name, 3, value);
}

/*--------------------------------------------------------------------------------*/
void glslSetParameter4i(GLSLprogram obj, const char* name, const GLSLint32* value){
	glslSetParameteri(obj, name, 4, value);
}


/*--------------------------------------------------------------------------------*/
void glslSetMatrixParameter(GLSLprogram obj, const char* name, GLSLint8 size, const GLSLfloat32* mat, GLSLbool trans){
	
	if (obj == 0) return;
	if (obj->is_valid == 0){
		glslFillLastError(GLSL_ERR_NOT_LINKED);
		glslCheckError(obj, 1);
		return;		
	}
	
	GLSL_Param param;
	glslGetParam(obj, name, &param);

	if (param.location != -1){
		if(size == 4) glUniformMatrix2fvARB(param.location,1,trans,mat);
		else if(size == 9) glUniformMatrix3fvARB(param.location,1,trans,mat);
		else if(size == 16)glUniformMatrix4fvARB(param.location,1,trans,mat);
		glslCheckError(obj, 0);
	}else{
		glslFillLastError(GLSL_ERR_NO_PARAMETER, name);
		glslCheckError(obj, 1);
	}

}


/*--------------------------------------------------------------------------------*/
void glslSetMatrixParameter4(GLSLprogram obj, const char* name, const GLSLfloat32* mat, GLSLbool trans){
	glslSetMatrixParameter(obj, name, 4, mat, trans);
}

/*--------------------------------------------------------------------------------*/
void glslSetMatrixParameter9(GLSLprogram obj, const char* name, const GLSLfloat32* mat, GLSLbool trans){
	glslSetMatrixParameter(obj, name, 9, mat, trans);
}


/*--------------------------------------------------------------------------------*/
void glslSetMatrixParameter16(GLSLprogram obj, const char* name, const GLSLfloat32* mat, GLSLbool trans){
	glslSetMatrixParameter(obj, name, 16, mat, trans);
}


/*--------------------------------------------------------------------------------*/
void glslSetTexture(GLSLprogram obj, const char* name, GLSLuint32 texture_unit){

	if (obj == 0) return;
	if (obj->is_valid == 0 || obj->program == 0){
		glslFillLastError(GLSL_ERR_NOT_LINKED);
		glslCheckError(obj, 1);
		return;		
	}
	
	GLSL_Param param;
	glslGetParam(obj, name, &param);

	if (param.location != -1){
		glUniform1iARB(param.location, (GLuint)texture_unit);
		glslCheckError(obj, 0);
	}else{
		glslFillLastError(GLSL_ERR_NO_PARAMETER, name);
		glslCheckError(obj, 1);
	}
}



/*--------------------------------------------------------------------------------*/
void glslEnableProgram(GLSLprogram obj){
	if (obj == 0) return;
	if (obj->program && obj->is_valid) glUseProgramObjectARB(obj->program);
}


/*--------------------------------------------------------------------------------*/
void glslDisableProgram(GLSLprogram obj){
	if (obj == 0) return;
	if (obj->program && obj->is_valid) glUseProgramObjectARB(0);
}

/*--------------------------------------------------------------------------------*/
void glslDisable(){
	glUseProgramObjectARB(0);
}

/*--------------------------------------------------------------------------------*/
int glslGetUniformParameterCount(GLSLprogram obj){
	if (obj == 0) return 0;
	return obj->param_count;
}

