#include "Shader.hpp"

#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
	std::string vCode, fCode;
	read(vertexPath, fragmentPath, vCode, fCode);
	id = compileAndLink(vCode, fCode);
}

Shader::~Shader()
{
	glDeleteProgram(id);
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
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_FOUND_OR_READ\n" << exception.what() << std::endl;
		// TODO fix shader failure not stopping the rest of the process
		//return;
	}
}

unsigned int Shader::compileAndLink(const std::string& vCode, const std::string& fCode)
{
	const char* vCode_c = vCode.c_str();
	const char* fCode_c = fCode.c_str();

	//-----COMPILE & LINK-----//
	unsigned int vID = 0;
	unsigned int fID = 0;
	int success = -1;
	char infoLog[512];

	// Vertex Shader
	vID = glCreateShader(GL_VERTEX_SHADER);
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
	fID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fID, 1, &fCode_c, nullptr);
	glCompileShader(fID);

	glGetShaderiv(fID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fID, 512, nullptr, infoLog);
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
	glUseProgram(id);
}

void Shader::set(const std::string& name, const bool value) const
{
	glUniform1i(location(name), static_cast<int>(value));
}

void Shader::set(const std::string& name, const int value) const
{
	glUniform1i(location(name), value);
}

void Shader::set(const std::string& name, const unsigned int value) const
{
	glUniform1ui(location(name), value);
}

void Shader::set(const std::string& name, const float value) const
{
	glUniform1f(location(name), value);
}

void Shader::set(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(location(name), 1, glm::value_ptr(value));
}

void Shader::set(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(location(name), 1, glm::value_ptr(value));
}

void Shader::set(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(location(name), 1, glm::value_ptr(value));
}

void Shader::set(const std::string& name, const glm::mat3& value) const
{
	glUniformMatrix3fv(location(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set(const std::string& name, const glm::mat4& value) const
{
	glUniformMatrix4fv(location(name), 1, GL_FALSE, glm::value_ptr(value));
}

unsigned Shader::location(const std::string& name) const
{
	return glGetUniformLocation(id, name.c_str());
}
