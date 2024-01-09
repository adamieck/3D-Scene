#include "Shader.h"
#include <glad/glad.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include "glm/gtc/type_ptr.inl"

Shader::Shader()
	: _rendererID(0)
{
}

Shader::~Shader()
{
    glDeleteProgram(_rendererID);
}

Shader& Shader::AddShader(const std::string& filepath, ShaderType type)
{
    SetShader(filepath, type);
    return *this;
}

void Shader::Build()
{
    _rendererID = CreateShader();
}

void Shader::SetShader(const std::string& filepath, ShaderType type)
{
    std::ifstream stream(filepath);
    std::string line;

    std::stringstream ss;

    while (std::getline(stream, line))
    {
        ss << line << '\n';
    }

    switch(type)
    {
    case ShaderType::VERTEX:
        vertexShaderSource = ss.str();
        break;
    case ShaderType::FRAGMENT:
        fragmentShaderSource = ss.str();
        break;
    case ShaderType::TESS_CTRL:
        tessCtrlShaderSource = ss.str();
        break;
    case ShaderType::TESS_EVAL:
        tessEvalShaderSource = ss.str();
        break;
	default:
        std::cout << "Unknown shader type\n";
    }
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    unsigned int shaderID = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shaderID, 1, &src, nullptr);
    glCompileShader(shaderID);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
        std::cout << "Failed to compile shader\n";
        std::cout << infoLog << std::endl;
        glDeleteShader(shaderID);
        return 0;
    }
    return shaderID;
}

int Shader::CreateShader()
{
    unsigned int vertexShaderID, fragmentShaderID, tessCtrlShaderID, tessEvalShaderID;
    unsigned int shaderProgram = glCreateProgram();
    if (!vertexShaderSource.empty())
    {
        vertexShaderID = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
        glAttachShader(shaderProgram, vertexShaderID);
    }
    if (!fragmentShaderSource.empty())
    {
        fragmentShaderID = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        glAttachShader(shaderProgram, fragmentShaderID);

    }
    if (!tessCtrlShaderSource.empty())
    {
        tessCtrlShaderID = CompileShader(GL_TESS_CONTROL_SHADER, tessCtrlShaderSource);
        glAttachShader(shaderProgram, tessCtrlShaderID);

    }
    if (!tessEvalShaderSource.empty())
    {
        tessEvalShaderID = CompileShader(GL_TESS_EVALUATION_SHADER, tessEvalShaderSource);
        glAttachShader(shaderProgram, tessEvalShaderID);

    }
        
    glLinkProgram(shaderProgram);
    glValidateProgram(shaderProgram);
    // check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Error linking shaders\n" << infoLog << std::endl;
    }
    //glDeleteShader(vertexShaderID);
    //glDeleteShader(fragmentShaderID);
    return shaderProgram;
}

void Shader::Bind() const
{
    glUseProgram(_rendererID);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

void Shader::SetUniform1f(const std::string& name, float v0)
{
    glUniform1f(GetUniformLocation(name), v0);
}

void Shader::SetUniform1i(const std::string& name, int v0)
{
    glUniform1i(GetUniformLocation(name), v0);
}

void Shader::SetUniform1d(const std::string& name, double v0)
{
    glUniform1d(GetUniformLocation(name), v0);
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
    glUniform3f(GetUniformLocation(name), v0, v1, v2);
}
void Shader::SetUniform3fv(const std::string& name, glm::vec3 v)
{
    glUniform3fv(GetUniformLocation(name), 1, &v[0]);
}
void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniform4fv(const std::string& name, glm::vec4 v)
{
    glUniform4fv(GetUniformLocation(name), 1, &v[0]);
}

void Shader::SetUniformMatrix4f(const std::string& name, const glm::mat4& mat)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

int Shader::GetUniformLocation(const std::string& name)
{
    int location = glGetUniformLocation(_rendererID, name.c_str());

    if(location == -1)
    {
        std::cout << "Warning: uniform '" << name << "' doesn't exist\n";
    }
    return location;
}

