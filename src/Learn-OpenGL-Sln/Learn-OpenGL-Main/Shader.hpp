#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>

class Shader
{
public:
	unsigned int ID;

	Shader(const std::string& vertexPath, const std::string& fragmentPath);

	void use() const;

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	
private:
	static void read(const std::string& vertexPath, const std::string& fragmentPath, std::string& vCode, std::string& fCode);
	static unsigned int compile_link(const std::string& vCode, const std::string& fCode);
};
#endif