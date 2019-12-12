#include "../Headers/ProgramLoader.h"
#include <iostream>

ProgramLoader::ProgramLoader() {
	programId = glCreateProgram();
}

ProgramLoader::~ProgramLoader() {
	deleteProgram();
}

ProgramLoader& ProgramLoader::attachShader(const char* filename, GLenum shaderType) noexcept(false) {
	// create a new shaderLoader and push it in the vector of 
	// ShaderLoaders after the attaching to the program succeeded
	ShaderLoader shLoader = ShaderLoader(filename, shaderType);
	
	GLuint shaderId;

	// read the shader from the source file and compile it, then get the it's id
	try {
		shaderId = shLoader
			.loadShader()
			.getShaderId();
	}
	catch (std::exception e) {
		// throw the error outside
		throw e;
	}

	glAttachShader(programId, shaderId);
	shaderLoaders.push_back(std::move(shLoader));

	// return a reference to this to enable method chaining
	return *this;
}

ProgramLoader& ProgramLoader::linkProgram() noexcept(false) {
	glLinkProgram(programId);

	GLint isLinked = GL_FALSE;
	glGetProgramiv(programId, GL_LINK_STATUS, &isLinked);

	// Linkage failed

	if (isLinked == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &maxLength);

		std::string outError("Program linkage failed\n");

		if (maxLength != 0)
		{
			std::vector<GLchar> errMsg(maxLength);
			glGetProgramInfoLog(programId, maxLength, &maxLength, &errMsg[0]);

			outError += &errMsg[0];
		}

		// if there is no linkage, there is no point to keeping the program and the shaders
		deleteProgram();

		throw std::exception(outError.c_str());
	}

	programLinkageSucceeded = true;
	return *this;
}

GLuint ProgramLoader::getProgramId() {
	return programId;
}

ProgramLoader& ProgramLoader::deleteShaders() {
	for (std::vector<ShaderLoader>::iterator it = shaderLoaders.begin(); it != shaderLoaders.end(); it++) {
		// if the linkage succeeded, we know the shaders have also been successfully attached to the program
		if (programLinkageSucceeded) {
			// before deleting the shader, make sure we detach it from the progam
			it->deleteShader(programId);
		}
		else {
			it->deleteShader();
		}
	}

	// remove all shaderLoader objects from the vector
	while (!shaderLoaders.empty()) {
		shaderLoaders.pop_back();
	}

	return *this;
}

void ProgramLoader::deleteProgram()
{
	deleteShaders();
	glDeleteProgram(programId);
}