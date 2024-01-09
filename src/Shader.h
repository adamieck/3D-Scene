#pragma once
#include <string>

#include "glm/glm.hpp"

enum class ShaderType
{
	NONE = -1, VERTEX = 0, FRAGMENT = 1,
	TESS_CTRL = 2, TESS_EVAL = 3
};

class Shader
{
private:
	unsigned int _rendererID;
public:
	std::string vertexShaderSource;
	std::string fragmentShaderSource;
	std::string tessCtrlShaderSource;
	std::string tessEvalShaderSource;

	Shader();
	~Shader();

	Shader& AddShader(const std::string& filepath, ShaderType type);
	void Build();
	void Bind() const;
	void Unbind() const;

	void SetShader(const std::string& filepath, ShaderType type);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	int CreateShader();

	// Uniforms
	void SetUniform1f(const std::string& name, float v0);
	void SetUniform1i(const std::string& name, int v0);
	void SetUniform1d(const std::string& name, double v0);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform3fv(const std::string& name, glm::vec3 v);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniform4fv(const std::string& name, glm::vec4 v);
	void SetUniformMatrix4f(const std::string& name, const glm::mat4& mat);


	int GetUniformLocation(const std::string& name);
};
