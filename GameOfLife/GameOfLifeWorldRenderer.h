#ifndef GAMEOFLIFEWORLDRENDERER_H
#define GAMEOFLIFEWORLDRENDERER_H

#include "Cell.h"

#include "../Buffer.h"
#include "../Shader.h"
#include "../VertexArrayObject.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <vector>

namespace GameOfLife {

class GameOfLifeWorld;

class GameOfLifeWorldRenderer
{
	struct Quad
	{
		glm::vec2 position;
		glm::vec3 color;
	};

public:

	GameOfLifeWorldRenderer();

	void initialize();

	void render(const GameOfLifeWorld& world) const;

private:

	void initializeShaders();

private:

	ShaderProgram program;
	VertexBufferObject instanceVertexBufferObject;
	VertexArrayObject quadVertexArrayObject;
	VertexBufferObject quadVertexBufferObject;

	mutable std::vector<Quad> quads;
};

} // namespace GameOfLife

#endif // GAMEOFLIFEWORLDRENDERER_H
