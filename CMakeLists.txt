project(evolution)
cmake_minimum_required(VERSION 2.8)

#add_subdirectory(sfgui)

set(SRC_LIST
	main.cpp
	Application.cpp
	Application.h
	BlockMap.h
	BlockMap.cpp
	Buffer.h
	GameOfLife/Cell.h
	GameOfLife/GameOfLifeWorld.cpp
	GameOfLife/GameOfLifeWorld.h
	GameOfLife/GameOfLifeWorldRenderer.cpp
	GameOfLife/GameOfLifeWorldRenderer.h
	Grid.h
	GridWorld/Cell.h
	GridWorld/GridWorld.cpp
	GridWorld/GridWorld.h
	GridWorld/GridWorldRenderer.cpp
	GridWorld/GridWorldRenderer.h
	ModuloIntDistribution.h
	PlantWorld/Cell.h
	PlantWorld/PlantWorld.cpp
	PlantWorld/PlantWorld.h
	PlantWorld/PlantWorldRenderer.cpp
	PlantWorld/PlantWorldRenderer.h
	Position.h
	PositionOffset.h
	ProbabilityGenerator.h
	Restorer.h
	Simulation.h
	Shader.h
	VertexArrayObject.h
)
add_executable(evolution ${SRC_LIST})

set_property(TARGET evolution PROPERTY CXX_STANDARD 14)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

find_package(OpenMP)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -Wall")

find_package(OpenGL REQUIRED)
target_link_libraries(evolution ${OPENGL_LIBRARIES})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
target_link_libraries(evolution ${GLEW_LIBRARIES})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
target_link_libraries(evolution ${SDL2_LIBRARY})
