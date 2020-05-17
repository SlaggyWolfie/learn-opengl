#include "Shader.hpp"

#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
	std::string vCode, fCode;
	read(vertexPath, fragmentPath, vCode, fCode);
	ID = compile_link(vCode, fCode);
}

void Shader::read(const std::string& vertexPath, const std::string& fragmentPath, std::string& vCode, std::string& fCode)
{
	std::ifstream vFile, fFile;

	// > ensure ifstream objects can throw exceptions
	vFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	//-----OPEN & READ-----//
	// Try to open and read the shader files
	// and place them into memory
	// ALTERNATIVE METHOD in MGE: read line-by-line and add it to a string
	try
	{
		// opening the files
		vFile.open(vertexPath);
		fFile.open(fragmentPath);

		// setting up streams (reading pipelines ?)
		std::stringstream vStream, fStream;

		// > read file's buffer contents into the stream
		vStream << vFile.rdbuf();
		fStream << fFile.rdbuf();

		// close
		vFile.close();
		fFile.close();

		// place into memory
		vCode = vStream.str();
		fCode = fStream.str();
	}
	catch (std::ifstream::failure& exception)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_FOUND_OR_READ" << std::endl;
		// TODO fix shader failure
		//return;
	}
}

unsigned int Shader::compile_link(const std::string& vCode, const std::string& fCode)
{
	const char* vCode_c = vCode.c_str();
	const char* fCode_c = fCode.c_str();

	//-----COMPILE & LINK-----//
	unsigned int vID = -1;
	unsigned int fID = -1;
	int success = -1;
	char infoLog[512];

	// Vertex Shader
	glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vID, 1, &vCode_c, nullptr);
	glCompileShader(vID);

	// print compilation errors
	glGetShaderiv(vID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vID, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Fragment Shader
	glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fID, 1, &fCode_c, nullptr);
	glCompileShader(fID);

	glGetShaderiv(fID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vID, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	const unsigned int ID = glCreateProgram();
	glAttachShader(ID, vID);
	glAttachShader(ID, fID);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vID);
	glDeleteShader(fID);

	return ID;
}

void Shader::use() const
{
	glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
}

void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
