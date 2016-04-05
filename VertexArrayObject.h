#ifndef VERTEXARRAYOBJECT_H
#define VERTEXARRAYOBJECT_H

#include <GL/glew.h>

class VertexArrayObject
{
public:
	VertexArrayObject()
		: index(0)
	{
	}

	explicit VertexArrayObject(GLuint index)
		: index(index)
	{
	}

	VertexArrayObject(const VertexArrayObject& other) = delete;

	VertexArrayObject(VertexArrayObject&& other)
	{
		index = other.index;
		other.index = 0;
	}

	VertexArrayObject& operator=(const VertexArrayObject& other) = delete;

	VertexArrayObject& operator=(VertexArrayObject&& other)
	{
		index = other.index;
		other.index = 0;
		return *this;
	}

	static VertexArrayObject generate()
	{
		GLuint index;
		glGenVertexArrays(1, &index);
		return VertexArrayObject(index);
	}

	~VertexArrayObject()
	{
		glDeleteVertexArrays(1, &index);
	}

	void bind() const
	{
		glBindVertexArray(index);
	}

	static void unbind()
	{
		glBindVertexArray(0);
	}

private:
	GLuint index;
};

#endif // VERTEXARRAYOBJECT_H
