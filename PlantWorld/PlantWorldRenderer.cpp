#include "PlantWorldRenderer.h"

#include "PlantWorld.h"

#include <algorithm>
#include <cstddef>

#include <iostream>

namespace PlantWorld {

PlantWorldRenderer::PlantWorldRenderer()
{
}

void PlantWorldRenderer::initialize()
{
	initializeShaders();

	instanceVertexBufferObject = VertexBufferObject::generate();

	GLfloat quadVertices[] = {
		-0.005f,  0.005f,
		0.005f, -0.005f,
		-0.005f, -0.005f,

		-0.005f,  0.005f,
		0.005f, -0.005f,
		0.005f,  0.005f,
	};

	quadVertexArrayObject = VertexArrayObject::generate();
	quadVertexBufferObject = VertexBufferObject::generate();

	quadVertexArrayObject.bind();
	quadVertexBufferObject.setData(quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

	instanceVertexBufferObject.bind();
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Quad), (GLvoid*)offsetof(Quad, position));
	glVertexAttribDivisor(1, 1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Quad), (GLvoid*)offsetof(Quad, color));
	glVertexAttribDivisor(2, 1);
	instanceVertexBufferObject.unbind();

	quadVertexArrayObject.unbind();
}

void PlantWorldRenderer::initializeShaders()
{
	VertexShader vertexShader(
		"#version 330 core\n"
		"layout (location = 0) in vec2 position;\n"
		"layout (location = 1) in vec2 offset;\n"
		"layout (location = 2) in vec3 color;\n"
		"out vec3 fColor;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = vec4(position + offset, 0.0f, 1.0f);\n"
		"	fColor = color;\n"
		"}\n"
	);
	FragmentShader fragmentShader(
		"#version 330 core\n"
		"in vec3 fColor;\n"
		"out vec4 color;\n"
		"void main()\n"
		"{\n"
		"	color = vec4(fColor, 1.0f);\n"
		"}\n"
	);

	program = ShaderProgram(vertexShader, fragmentShader);
}

void PlantWorldRenderer::render(const PlantWorld& world) const
{
	quads.clear();
	energies.clear();
	ages.clear();
	sizes.clear();
	maxSizes.clear();

	const Grid<Cell>* grid = world.currentGrid.get();
	int rowCount = world.rowCount;
	int columnCount = world.columnCount;

	for (int r = 0; r < rowCount; ++ r) {
		for (int c = 0; c < columnCount; ++c) {
			const Cell& cell = grid->at(r, c);
			if (cell.hasPlant) {
				energies.push_back(cell.plant.energy);
				ages.push_back(cell.plant.age);
				sizes.push_back(cell.plant.size);
				maxSizes.push_back(cell.plant.maxSize);
			}
		}
	}

	int medianAge;
	if (!ages.empty()) {
		int i = ages.size()/2;
		std::nth_element(ages.begin(), ages.begin() + i, ages.end());
		medianAge = ages[i];
	}

	for (int r = 0; r < rowCount; ++ r) {
		GLfloat y = 0.95f - GLfloat(1.90 * r) / rowCount;
		for (int c = 0; c < columnCount; ++c) {
			GLfloat x = 0.95f - GLfloat(1.90 * c) / columnCount;
			const Cell& cell = grid->at(r, c);
			if (cell.hasPlant) {
				quads.push_back({{x, y}, {std::min((cell.plant.age - medianAge + 128) / 256.0f, 1.0f), 1.0f, 0.0f}});
			}
		}
	}

	program.use();
	quadVertexArrayObject.bind();
	instanceVertexBufferObject.setData(quads, GL_DYNAMIC_DRAW);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, quads.size());
	quadVertexArrayObject.unbind();

	std::cout << "energy:   " << medianElement(energies) << std::endl;
	std::cout << "age:      " << medianElement(ages) << std::endl;
	std::cout << "sizes:    " << medianElement(sizes) << std::endl;
	std::cout << "maxSizes: " << medianElement(maxSizes) << std::endl;
}

std::string PlantWorldRenderer::medianElement(std::vector<int>& v, std::string empty) const
{
	if (v.empty()) {
		return empty;
	} else {
		// NOTE: fuck evens
		int i = v.size()/2;
		std::nth_element(v.begin(), v.begin() + i, v.end());
		return std::to_string(v[i]);
	}
}

} // namespace PlantWorld
