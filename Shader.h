#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

#include <stdexcept>
#include <string>

class VertexShader
{
public:
	VertexShader(const GLchar *code)
	{
		index = glCreateShader(GL_VERTEX_SHADER);
		if (index == 0) {
			throw std::runtime_error("Cannot create vertex shader.");
		}
		glShaderSource(index, 1, &code, NULL);
		glCompileShader(index);
		GLint success;
		glGetShaderiv(index, GL_COMPILE_STATUS, &success);
		if (!success) {
			GLint infoLogLength;
			glGetShaderiv(index, GL_INFO_LOG_LENGTH, &infoLogLength);
			std::basic_string<GLchar> infoLog(infoLogLength, '\0');
			glGetShaderInfoLog(index, infoLog.length(), NULL, &infoLog[0]);
			throw std::logic_error("Cannot compile vertex shader: " + infoLog);
		}
	}

	VertexShader(VertexShader &&other)
		: index(other.index)
	{
		other.index = 0;
	}

	~VertexShader()
	{
		glDeleteShader(index);
	}

private:
	GLuint index;

	friend class ShaderProgram;
};

class FragmentShader
{
public:
	FragmentShader(const GLchar *code)
	{
		index = glCreateShader(GL_FRAGMENT_SHADER);
		if (index == 0) {
			throw std::runtime_error("Cannot create fragment shader.");
		}
		glShaderSource(index, 1, &code, NULL);
		glCompileShader(index);
		GLint success;
		glGetShaderiv(index, GL_COMPILE_STATUS, &success);
		if (!success) {
			GLint infoLogLength;
			glGetShaderiv(index, GL_INFO_LOG_LENGTH, &infoLogLength);
			std::basic_string<GLchar> infoLog(infoLogLength, '\0');
			glGetShaderInfoLog(index, infoLog.length(), NULL, &infoLog[0]);
			throw std::logic_error("Cannot compile fragment shader: " + infoLog);
		}
	}

	FragmentShader(FragmentShader &&other)
		: index(other.index)
	{
		other.index = 0;
	}

	~FragmentShader()
	{
		glDeleteShader(index);
	}

private:
	GLuint index;

	friend class ShaderProgram;
};

class ShaderProgram
{
public:
	ShaderProgram()
		: index(0)
	{
	}

	ShaderProgram(const VertexShader &vertexShader, const FragmentShader &fragmentShader)
	{
		index = glCreateProgram();
		if (index == 0) {
			throw std::runtime_error("Cannot create program.");
		}
		glAttachShader(index, vertexShader.index);
		glAttachShader(index, fragmentShader.index);
		glLinkProgram(index);
		GLint success;
		glGetProgramiv(index, GL_LINK_STATUS, &success);
		if (!success) {
			GLint infoLogLength;
			glGetProgramiv(index, GL_INFO_LOG_LENGTH, &infoLogLength);
			std::basic_string<GLchar> infoLog(infoLogLength, '\0');
			glGetProgramInfoLog(index, infoLog.length(), NULL, &infoLog[0]);
			throw std::logic_error("Cannot link program: " + infoLog);
		}
	}

	ShaderProgram(ShaderProgram &&other)
		: index(other.index)
	{
		other.index = 0;
	}

	ShaderProgram& operator=(ShaderProgram &&other)
	{
		index = other.index;
		other.index = 0;
		return *this;
	}

	~ShaderProgram()
	{
		glDeleteProgram(index);
	}

	void use() const
	{
		glUseProgram(index);
	}

private:
	GLuint index;
};

#endif // SHADER_H
