#pragma once
#include "ShaderLoader.h"

class ProgramLoader 
{
	GLuint programId = 0;
	bool programLinkageSucceeded = false;
	std::vector<ShaderLoader> shaderLoaders;

public:
	ProgramLoader();
	~ProgramLoader();

	ProgramLoader& attachShader(const char *filename, GLenum shaderType) noexcept(false);
	ProgramLoader& linkProgram() noexcept(false);
	GLuint getProgramId();

	ProgramLoader& deleteShaders();
	void deleteProgram();
};