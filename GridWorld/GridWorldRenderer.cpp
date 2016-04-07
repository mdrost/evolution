#include "GridWorldRenderer.h"

#include "GridWorld.h"

#include "../Position.h"
#include "../Restorer.h"

#include <cstddef>

#include <iostream>
#include <iomanip>

GridWorldRenderer::GridWorldRenderer()
{
}

void GridWorldRenderer::initialize()
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

void GridWorldRenderer::initializeShaders()
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

void GridWorldRenderer::render(const GridWorld& gridWorld) const
{
	using namespace std;

	quads.clear();
	plantStatistics.clear();
	herbivoreStatistics.clear();
	carnivoreStatistics.clear();

	int rows = gridWorld.rows;
	int columns = gridWorld.columns;
	const BlockMap<Cell, GridWorld::BLOCK_ROWS, GridWorld::BLOCK_COLUMNS>& cellBlocks = gridWorld.cellBlocks;
	Position p;
	for (int r = 0; r < cellBlocks.rows(); ++r) {
		Restorer<int> colRestorer(p.col);
		for (int c = 0; c < cellBlocks.columns(); ++c) {
			const Block<Cell>& block = cellBlocks.block(r, c);
			//std::uniform_real_distribution<float> cd(0.0f, 1.0f);
			//glm::vec3 color(cd(mt), cd(mt), cd(mt));
			Restorer<int> rowRestorer(p.row);
			for (int i = 0; i < block.rows(); ++i) {
				Restorer<int> colRestorer(p.col);
				for (int j = 0; j < block.columns(); ++j) {
					const Cell &cell = block.cell(i, j);
					GLfloat qx = 0.95f - GLfloat(1.90 * p.col) / columns;
					GLfloat qy = 0.95f - GLfloat(1.90 * p.row) / rows;
					if (cell.hasPlant()) {
						quads.push_back({{qx, qy}, {0.0f, 1.0f, 0.0f}});
						//quads.push_back({{qx, qy}, color});
						plantStatistics.record(*cell.plant());

						if (cell.hasHerbivore()) {
							quads.push_back({{qx, qy}, {0.0f, 0.1f, 1.0f}});
							herbivoreStatistics.record(*cell.herbivore());
						}
						if (cell.hasCarnivore()) {
							quads.push_back({{qx, qy}, {1.0f, 0.1f, 0.0f}});
							carnivoreStatistics.record(*cell.carnivore());
						}
					} else {
						if (cell.hasHerbivore()) {
							quads.push_back({{qx, qy}, {0.0f, 0.0f, 1.0f}});
							herbivoreStatistics.record(*cell.herbivore());
						}
						if (cell.hasCarnivore()) {
							quads.push_back({{qx, qy}, {1.0f, 0.0f, 0.0f}});
							carnivoreStatistics.record(*cell.carnivore());
						}
					}
					/*if (cell.accident()) {
						quads.push_back({{qx, qy}, {1.0f, 1.0f, 0.0f}});
					}*/
					p.col += 1;
				}
				p.row += 1;
			}
			p.col += block.columns();
		}
		p.row += GridWorld::BLOCK_ROWS;
	}

	cout << endl;
	cout << "      plant herbi carni" << endl;
	cout << "count" << setw(6)<<right << plantStatistics.energies.size() << setw(6)<<right << herbivoreStatistics.energies.size() << setw(6)<<right << carnivoreStatistics.energies.size() << endl;
	cout << "energ" << setw(6)<<right << plantEnergy() << setw(6)<<right << herbivoreEnergy() << setw(6)<<right << carnivoreEnergy() << endl;
	cout << "repro" << setw(6)<<right << plantReproduction() << setw(6)<<right << herbivoreReproduction() << setw(6)<<right << carnivoreReproduction() <<  std::endl;
	cout << "offsp" << setw(6)<<right << plantOffspring() << setw(6)<<right << herbivoreOffspring() << setw(6)<<right << carnivoreOffspring() << std::endl;
	cout << "decre" << setw(6)<<right << plantGeneDecrementFactor() << setw(6)<<right << herbivoreGeneDecrementFactor() << setw(6)<<right << carnivoreGeneDecrementFactor() << std::endl;
	cout << "stabi" << setw(6)<<right << plantGeneStabilizeFactor() << setw(6)<<right << herbivoreGeneStabilizeFactor() << setw(6)<<right << carnivoreGeneStabilizeFactor() << std::endl;
	cout << "incre" << setw(6)<<right << plantGeneIncrementFactor() << setw(6)<<right << herbivoreGeneIncrementFactor() << setw(6)<<right << carnivoreGeneIncrementFactor() << std::endl;
	cout << "feast" << setw(6)<<right << "-" << setw(6)<<right << herbivoreFeastSize() << setw(6)<<right << "-" << endl;
	cout << endl;

	program.use();
	quadVertexArrayObject.bind();
	instanceVertexBufferObject.setData(quads, GL_DYNAMIC_DRAW);
	//std::cout << "size: " << quads.size() << std::endl;
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, quads.size());
	quadVertexArrayObject.unbind();
}

std::string GridWorldRenderer::medianElement(std::vector<int>& v, std::string empty)
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

void GridWorldRenderer::OrganismStatistics::clear()
{
	energies.clear();
	reproductionEnergies.clear();
	offspringEnergies.clear();
	geneDecrementFactors.clear();
	geneStabilizeFactors.clear();
	geneIncrementFactors.clear();
}

void GridWorldRenderer::OrganismStatistics::record(const Organism &organism)
{
	energies.push_back(organism.energy);
	reproductionEnergies.push_back(organism.reproductionEnergy);
	offspringEnergies.push_back(organism.offspringEnergy);
	geneDecrementFactors.push_back(organism.geneDecrementFactor);
	geneStabilizeFactors.push_back(organism.geneStabilizeFactor);
	geneIncrementFactors.push_back(organism.geneIncrementFactor);
}

void GridWorldRenderer::HerbivoreStatistics::clear()
{
	OrganismStatistics::clear();
	feastSizes.clear();
}

void GridWorldRenderer::HerbivoreStatistics::record(const Herbivore &herbivore)
{
	OrganismStatistics::record(herbivore);
	feastSizes.push_back(herbivore.feastSize);
}
