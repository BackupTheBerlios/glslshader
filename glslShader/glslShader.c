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

#include "glslShader.h"

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

	glslSetErrorCallback(error_callback, NULL);
	
	shader = glslCreateProgram();
// 	glslAttachShader(shader, "shader/fDebugCSGWriter.glsl.cg", GL_FRAGMENT_SHADER_ARB);
	glslAttachShader(shader, "glsl/vDepthPNG.gl", GL_VERTEX_SHADER_ARB);
	glslLinkProgram(shader);

// 	glslEnableProgram(shader);

	
	return 0;
	
}
#endif


// Internal used functions
void glslEmptyLastError();
void glslFillLastError(const char* fmt, ...);
void glslCheckError(GLSLprogram obj, bool internal = false);
void glslGetParam(GLSLprogram obj, const char* name, GLSL_Param* param);
char* glslScanAttributes(GLSLprogram obj, char* prog, GLSL_Attr** attr, int* count);


/**
 * Store here the last error string
 **/
char	___glsl_last_error_string[4096];

/**
 * Store 1 if there is an error stored in variable or 0 otherwise
 **/
int		___glsl_has_last_error = 0;

/**
 * Store here the pointer to specified callback function.
 **/
GLSLerrorCallback ___glsl_error_callback = NULL;


/**
 * Store here the specified callback parameter which will be given
 * to the callback fucntion.
 **/
void*			  ___glsl_error_callback_param = NULL;


/**
 * This variables do pushing and poping of glsl programs on the stack
 **/
GLhandleARB	_glsl_Program_Stack[GLSL_STACK_SIZE];
int  				_glsl_Program_StackPos = 0;

//--------------------------------------------------------------------------------
void glslPush(){
	if (_glsl_Program_StackPos > GLSL_STACK_SIZE - 1){
		_glsl_Program_StackPos = GLSL_STACK_SIZE - 1;
	}
	_glsl_Program_Stack[_glsl_Program_StackPos] = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
	_glsl_Program_StackPos++;
}


//--------------------------------------------------------------------------------
void glslPop(){
	if (_glsl_Program_StackPos < 1){
		return;
	}
	_glsl_Program_StackPos--;
	glUseProgramObjectARB(_glsl_Program_Stack[_glsl_Program_StackPos]);
}

				 
//--------------------------------------------------------------------------------
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

	if (obj == NULL) return;

	// is program valid
	if (obj->program == 0 || obj->is_valid == false){
		glslFillLastError(GLSL_ERR_NOT_VALID_PROGRAM);
		glslCheckError(obj, true);
		return;		
	}

	// has the program got any parameters
	if (obj->params == NULL){
		glslFillLastError(GLSL_ERR_NO_PARAMETERS);
		glslCheckError(obj, true);
		return;		
	}
	
	param->location = glGetUniformLocationARB(obj->program, (const GLcharARB*)name);

}

//--------------------------------------------------------------------------------
GLSLprogram glslCreateProgram(){

	GLSLprogram prog = NULL;

	prog = (GLSLprogram)malloc(sizeof(GLSL_Shader));
	if (prog == NULL) return NULL;
	
	// it is not valid at now
	prog->is_valid = false;

	// setup all parameters
	prog->params = NULL;
	prog->param_count = 0;
	
	// Create program through OpenGL's context
	prog->program = glCreateProgramObjectARB();

	if (prog->program == 0){
		glslFillLastError(GLSL_ERR_CAN_NOT_CREATE);
		glslCheckError(prog, true);
	}else{
		glslEmptyLastError();
	}
	
	return prog;
	
}


//--------------------------------------------------------------------------------
void glslAttachShader(GLSLprogram obj, const char* filename, GLenum shaderType){

	if (obj == NULL) return;

	char*	data = NULL;
	int 	size = 0;
	FILE* 	file = NULL;

	// check for right shader type
	if (shaderType != GL_VERTEX_SHADER_ARB && shaderType != GL_FRAGMENT_SHADER_ARB){
		glslFillLastError(GLSL_ERR_WRONG_SHADER_TYPE);
		glslCheckError(obj, true);
		return;		
	}

	if (obj->program == 0){
		glslFillLastError(GLSL_ERR_NOT_VALID_PROGRAM);
		glslCheckError(obj, true);
		return;		
	}

	
	// open the file
	file = fopen(filename,"rb");
	if(!file) {
		glslFillLastError(GLSL_ERR_CANNOT_OPEN_FILE, filename);
		glslCheckError(obj, true);
		return;
	}

	// get the size of the file and read it
	fseek(file,0,SEEK_END);
	size = ftell(file);
	data = (char*)malloc(sizeof(char)*(size + 1));
	memset(data, 0, sizeof(char)*(size + 1));
	fseek(file,0,SEEK_SET);
	fread(data,1,size,file);
	fclose(file);

#if 0
	// read all attributes from the program
	GLSL_Attr* attr = NULL;
	int count = 0;
	char* ndata = glslScanAttributes(obj, data, &attr, &count);
#endif

	// compile the source
	GLint length = (GLint)strlen(data);
	GLhandleARB shader = glCreateShaderObjectARB(shaderType);
	glShaderSourceARB(shader, 1, (const GLcharARB**)&data, &length);
	glCompileShaderARB(shader);
	glAttachObjectARB(obj->program, shader);
	glDeleteObjectARB(shader);

#if 0
	// Now combine all used attributes and copy them to program object
	for (int i=0; i < count; i++){
		glBindAttribLocationARB(obj->program, attr[i].index, attr[i].name);
	}
	free(ndata);
#endif
	
	// release used memory
	free(data);
	
	glslCheckError(obj);

}

//--------------------------------------------------------------------------------
void glslAttachVertexShader(GLSLprogram obj, const char* filename){
	glslAttachShader(obj, filename, GL_VERTEX_SHADER_ARB);
}

//--------------------------------------------------------------------------------
void glslAttachFragmentShader(GLSLprogram obj, const char* filename){
	glslAttachShader(obj, filename, GL_FRAGMENT_SHADER_ARB);
}


//--------------------------------------------------------------------------------
void glslBindAttribute(GLSLprogram obj, int attrIndex, char* name){

	if (obj == NULL)return;
	
	glBindAttribLocationARB(obj->program, attrIndex, name);

	// if already linked, so link again
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
char* glslScanAttributes(GLSLprogram obj, char* prog, GLSL_Attr** attr, int* count){

	if (attr == NULL || count == NULL){
		return prog;
	}

	// define the buffer which will hold our code to be returned
	char* new_prog = NULL;
	new_prog = (char*)malloc(strlen(prog)*sizeof(char));
	if (new_prog == NULL){
		glslFillLastError(GLSL_ERR_OUT_OF_MEMORY);
		glslCheckError(obj, true);
		return prog;
	}
	memset(new_prog, 0, sizeof(char)*strlen(prog));
	
	// temporary attributes storing
	GLSL_Attr attrTemp[64];
	int attrCount = 0;
		
	// scan the program
	char* source = prog;
	char* dest = new_prog;
	while (1){
		// scan the code for attribute line
		char* at = strstr(source, "attribute ");

		// if nothing found, so break the loop
		if (at == NULL){
			static int i = 0;
			i++;
			fprintf(stderr, "%i - %i\n", i, source - prog);
			// copy now the rest to the new source
			strcpy(dest, source);
			break;
		}

		// now at has got right value, so scan now for the index number
		int index = -1;
		int space_count = 0;
		char name[GLSL_LONGEST_NAME+1];
		int name_index = 0;
		memset(name, 0, sizeof(char) * (GLSL_LONGEST_NAME+1));
		char* s = at;
		while (*s != ';' && *s != '\0'){
			// ok we found the start of index number
			if (*s == ':'){
				s++;
				while (*s == ' ' && *s != '\0')s++;				
				if (*s == '\0')break;
				
				// now the next four characters must be ATTR
				if (*(s++) != 'A') break;
				if (*(s++) != 'T') break;
				if (*(s++) != 'T') break;
				if (*(s++) != 'R') break;

				// so if we here so we found the ATTR string
				// now scan for the number
				char buffer[5];
				memset(buffer, 0, 5*sizeof(char));
				int cb = 0;
				while (*s != '\0' && *s != ' ' && *s != ';' && cb < 5){
					buffer[cb] = *s;
					s++;
					cb++;
				}

				// No number so error
				if (strlen(buffer) == 0) break;

				// get the scanned number
				index = atoi(buffer);

				
			// we do not see any index number
			}else{
				
				// now scan for the name of attribute
				// the name will be found after the second space character
				if (*s == ' ' || *s == ';' || *s == ':')space_count++;
				if (space_count >= 2){
					// the next string will be the name
					if (space_count == 3 && *s != ' ' && *s != ':' && *s != ';'){
						name[name_index++] = *s;
					}
				}
			}
			
			// copy the source to destination
			*(dest++) = *s;

			// next character
			if (*s != ';') s++;
			
		} // while (;)

		// from here we know the name of the attribute and if the
		// index was given also the index number

		// if we have no name, so this is an error
		if (name_index == 0){
			strcpy(dest, source);
			break;
		}

		// if we have index and the name, so copy it to temporary array
		attrTemp[attrCount].index = index;
		strcpy(attrTemp[attrCount].name, name);
		attrCount++;

		// know set the source pointer to right position
		// if the source code was correct
		source = s;
		
	}

	// now copy all found attributes
	if (attrCount > 0){
		*attr = (GLSL_Attr*)malloc(sizeof(GLSL_Attr) * attrCount);
		if (*attr != NULL){
			*count = attrCount;
			for (int i=0; i < attrCount; i++){
				((*attr)[i]).index = attrTemp[i].index;
				strcpy(((*attr)[i]).name, attrTemp[i].name);
			}
		}
	}
	
	return new_prog;
}


//--------------------------------------------------------------------------------
void glslLinkProgram(GLSLprogram obj){

	if (obj == NULL) return;
	
	// check if program is valid
	if (obj->program == 0){
		glslFillLastError(GLSL_ERR_NOT_VALID_PROGRAM);
		glslCheckError(obj, true);
		return;		
	}

	// Link the program
	glLinkProgramARB(obj->program);
	GLint linked;
	glGetObjectParameterivARB(obj->program, GL_OBJECT_LINK_STATUS_ARB, &linked);
	if(!linked) {
		glslCheckError(obj);
		return;
	}

	obj->is_valid = true;
	glslEmptyLastError();


	// Get parameters count
	GLint count = 0;
	glGetObjectParameterivARB(obj->program, GL_OBJECT_ACTIVE_UNIFORMS_ARB, &count);
	
	// get all uniform parameters in the program
	if (count > 0){
		obj->param_count = count;
		obj->params = (GLSL_Param*)malloc(count* sizeof(GLSL_Param));
		memset(obj->params, 0, count* sizeof(GLSL_Param));
			
		for (GLuint i=0; i < (GLuint)count; i++){
			GLsizei length = 0;
			GLint size = 0;
			GLenum type;
			GLcharARB name[GLSL_LONGEST_NAME];
			memset(name, 0, GLSL_LONGEST_NAME*sizeof(GLcharARB));
			
			glGetActiveUniformARB(obj->program,i, GLSL_LONGEST_NAME, &length, &size, &type, &(name[0]));

			// Copy name for the uniform parameter
			if (length < GLSL_LONGEST_NAME)
				memcpy(obj->params[i].name, name, sizeof(char)*length);
			else
				memcpy(obj->params[i].name, name, sizeof(char)*GLSL_LONGEST_NAME);

			// copy other data
			obj->params[i].length = size;
			obj->params[i].type = type;
			obj->params[i].location = glGetUniformLocationARB(obj->program, name);
		}
	}
	
}


//--------------------------------------------------------------------------------
void glslDeleteProgram(GLSLprogram obj){
	if (obj == NULL) return;
	if (obj->program) glDeleteObjectARB(obj->program);

	if (obj->params != NULL)free(obj->params);
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
 *  name" or something else). External errors are produced by GLSL
 * and will be tested here.
 **/
void glslCheckError(GLSLprogram obj, bool internal){
	
	bool occurs = internal;
	
	if (!internal){
		int length = 0;
		static char error[4096];
		glGetInfoLogARB(obj->program, 4096, &length, error);

		glslEmptyLastError();

		if (length > 0){
			strcpy(___glsl_last_error_string, error);
			___glsl_has_last_error = 1;

			if (strstr(error, "warning:") == NULL){
				occurs = true;
			}
		}		
	}
	
	if (___glsl_error_callback && occurs){
		___glsl_error_callback(obj, ___glsl_error_callback_param);
	}

	
}


//--------------------------------------------------------------------------------
int	glslHasError(){
	return ___glsl_has_last_error;
}


//--------------------------------------------------------------------------------
void glslSetErrorCallback(GLSLerrorCallback pCallback, void* callbackParam){
	___glsl_error_callback = pCallback;
	___glsl_error_callback_param = callbackParam;
}


//--------------------------------------------------------------------------------
void glslSetParameterf(GLSLprogram obj, const char* name, int size, const float* value){

	if (obj == NULL) return;
	if (obj->is_valid == false){
		glslFillLastError(GLSL_ERR_NOT_LINKED);
		glslCheckError(obj, true);
		return;		
	}
	
	GLSL_Param param;
	glslGetParam(obj, name, &param);

	if (param.location != -1){
		if(size == 1) glUniform1fvARB(param.location,1,value);
		else if(size == 2) glUniform2fvARB(param.location,1,value);
		else if(size == 3) glUniform3fvARB(param.location,1,value);
		else if(size == 4) glUniform4fvARB(param.location,1,value);
		glslCheckError(obj);
	}else{
		glslFillLastError(GLSL_ERR_NO_PARAMETER, name);
		glslCheckError(obj, true);
	}
	
}


//--------------------------------------------------------------------------------
void glslSetParameteri(GLSLprogram obj, const char* name, int size, const int* value){

	if (obj == NULL) return;
	if (obj->is_valid == false){
		glslFillLastError(GLSL_ERR_NOT_LINKED);
		glslCheckError(obj, true);
		return;		
	}
	
	GLSL_Param param;
	glslGetParam(obj, name, &param);

	if (param.location != -1){
		if(size == 1) glUniform1ivARB(param.location,1,value);
		else if(size == 2) glUniform2ivARB(param.location,1,value);
		else if(size == 3) glUniform3ivARB(param.location,1,value);
		else if(size == 4) glUniform4ivARB(param.location,1,value);
		glslCheckError(obj);
	}else{
		glslFillLastError(GLSL_ERR_NO_PARAMETER, name);
		glslCheckError(obj, true);
	}
}



//--------------------------------------------------------------------------------
void glslSetMatrixParameter(GLSLprogram obj, const char* name, int size, const float* mat, bool trans){
	
	if (obj == NULL) return;
	if (obj->is_valid == false){
		glslFillLastError(GLSL_ERR_NOT_LINKED);
		glslCheckError(obj, true);
		return;		
	}
	
	GLSL_Param param;
	glslGetParam(obj, name, &param);

	if (param.location != -1){
		if(size == 4) glUniformMatrix2fvARB(param.location,1,trans,mat);
		else if(size == 9) glUniformMatrix3fvARB(param.location,1,trans,mat);
		else if(size == 16)glUniformMatrix4fvARB(param.location,1,trans,mat);
		glslCheckError(obj);
	}else{
		glslFillLastError(GLSL_ERR_NO_PARAMETER, name);
		glslCheckError(obj, true);
	}

}



//--------------------------------------------------------------------------------
void glslSetTexture(GLSLprogram obj, const char* name, GLuint texture_unit){

	if (obj == NULL) return;
	if (obj->is_valid == false || obj->program == 0){
		glslFillLastError(GLSL_ERR_NOT_LINKED);
		glslCheckError(obj, true);
		return;		
	}
	
	GLSL_Param param;
	glslGetParam(obj, name, &param);

	if (param.location != -1){
		glUniform1iARB(param.location, texture_unit);
		glslCheckError(obj);
	}else{
		glslFillLastError(GLSL_ERR_NO_PARAMETER, name);
		glslCheckError(obj, true);
	}
}



//--------------------------------------------------------------------------------
void glslEnableProgram(GLSLprogram obj){
	if (obj == NULL) return;
	if (obj->program && obj->is_valid) glUseProgramObjectARB(obj->program);
}


//--------------------------------------------------------------------------------
void glslDisableProgram(GLSLprogram obj){
	if (obj == NULL) return;
	if (obj->program && obj->is_valid) glUseProgramObjectARB(0);
}

//--------------------------------------------------------------------------------
void glslDisable(){
	glUseProgramObjectARB(0);
}

//--------------------------------------------------------------------------------
int glslGetUniformParameterCount(GLSLprogram obj){
	if (obj == NULL) return 0;
	return obj->param_count;
}

