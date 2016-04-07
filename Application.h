#ifndef APPLICATION_H
#define APPLICATION_H

#include "GridWorld.h"

#include <SDL2/SDL.h>

class Application
{
public:
	Application();

	void run();

private:
	bool initialize();

	void handleEvents();

	void update();

	void render();

	void cleanup();

private:
	GridWorld world;

	bool running;
	int frameCounter;
	int updateCounter;

	SDL_Window *window;
	SDL_GLContext glContext;
};

#endif // APPLICATION_H
