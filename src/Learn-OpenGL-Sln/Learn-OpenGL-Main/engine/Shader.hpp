﻿#pragma once
#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <glm/glm.hpp>

class Shader
{
public:
	unsigned int id;

	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	Shader(const std::string& path);
	~Shader();

	void use() const;

	void set(const std::string& name, bool value) const;
	void set(const std::string& name, int value) const;
	void set(const std::string& name, unsigned int value) const;
	void set(const std::string& name, float value) const;
	void set(const std::string& name, const glm::vec2& value) const;
	void set(const std::string& name, const glm::vec3& value) const;
	void set(const std::string& name, const glm::vec4& value) const;
	void set(const std::string& name, const glm::mat3& value) const;
	void set(const std::string& name, const glm::mat4& value) const;
	
private:
	unsigned int location(const std::string& name) const;
	
	static void read(const std::string& vertexPath, const std::string& fragmentPath, std::string& vCode, std::string& fCode);
	static unsigned int compileAndLink(const std::string& vCode, const std::string& fCode);
};
#endif