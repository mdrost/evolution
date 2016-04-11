#ifndef PLANTWORLD_PLANTWORLDRENDERER_H
#define PLANTWORLD_PLANTWORLDRENDERER_H

#include "../Buffer.h"
#include "../Shader.h"
#include "../VertexArrayObject.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <vector>

namespace PlantWorld {

class PlantWorld;

class PlantWorldRenderer
{
	struct Quad
	{
		glm::vec2 position;
		glm::vec3 color;
	};

public:

	PlantWorldRenderer();

	void initialize();

	void render(const PlantWorld& world) const;

private:

	void initializeShaders();

	std::string medianElement(std::vector<int>& v, std::string empty = "-") const;

private:

	ShaderProgram program;
	VertexBufferObject instanceVertexBufferObject;
	VertexArrayObject quadVertexArrayObject;
	VertexBufferObject quadVertexBufferObject;

	mutable std::vector<Quad> quads;
	mutable std::vector<int> energies;
	mutable std::vector<int> ages;
	mutable std::vector<int> sizes;
	mutable std::vector<int> maxSizes;
};

} // namespace PlantWorld

#endif // PLANTWORLD_PLANTWORLDRENDERER_H
