#ifndef BUFFER_H
#define BUFFER_H

#include <GL/glew.h>

#include <vector>

class GlBuffer
{
public:
	GlBuffer()
		: index(0)
	{
	}

	explicit GlBuffer(GLuint index)
		: index(index)
	{
	}

	GlBuffer(const GlBuffer& other) = delete;

	GlBuffer(GlBuffer&& other)
	{
		index = other.index;
		other.index = 0;
	}

	GlBuffer& operator=(const GlBuffer& other) = delete;

	GlBuffer& operator=(GlBuffer&& other)
	{
		index = other.index;
		other.index = 0;
		return *this;
	}

	~GlBuffer()
	{
		glDeleteBuffers(1, &index);
	}

protected:
	GLuint index;
};

class VertexBufferObject : public GlBuffer
{
public:
	using GlBuffer::GlBuffer;

	static VertexBufferObject generate()
	{
		GLuint buffer;
		glGenBuffers(1, &buffer);
		return VertexBufferObject(buffer);
	}

	template <typename PodType, int N>
	static VertexBufferObject generateWithData(PodType (&data)[N], GLenum usage)
	{
		VertexBufferObject vbo = generate();
		vbo.setData(data, usage);
		return vbo;
	}

	void bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, index);
	}

	static void unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	template <typename PodType, int N>
	void setData(PodType (&data)[N], GLenum usage) const
	{
		bind();
		glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, usage);
	}

	template <typename PodType>
	void setData(const std::vector<PodType> &data, GLenum usage) const
	{
		bind();
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(PodType), &data[0], usage);
	}
};

class ElementArrayBuffer : public GlBuffer
{
public:
	using GlBuffer::GlBuffer;

	static ElementArrayBuffer generate()
	{
		GLuint buffer;
		glGenBuffers(1, &buffer);
		return ElementArrayBuffer(buffer);
	}

	void bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);
	}

	static void unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	template <typename PodType, int N>
	void setData(PodType (&data)[N], GLenum usage) const
	{
		bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(data), data, usage);
	}
};

#endif // BUFFER_H
