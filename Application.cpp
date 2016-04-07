#include "Application.h"

#include <iostream>

Application::Application()
	: running(false)
{
}

void Application::run()
{
	if (!initialize()) {
		return;
	}

	Uint64 frameTime = SDL_GetPerformanceFrequency() / 1;
	Uint64 time = SDL_GetPerformanceCounter();

	// Display first frame
	render();

	while (running) {
		frameCounter += 1;

		handleEvents();

		int u = 1;
		update();
		while ((SDL_GetPerformanceCounter() - time) < frameTime) {
			++u;
			update();
		}

		time = SDL_GetPerformanceCounter();

		std::cout << "updates: " << u << std::endl;
		render();
	}

	cleanup();
}

bool Application::initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	window = SDL_CreateWindow("Evolution",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!window) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return false;
	}

	glContext = SDL_GL_CreateContext(window);
	if (!glContext) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return false;
	}

	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	if (SDL_GL_SetSwapInterval(1) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
	}

	running = true;
	frameCounter = 1;
	updateCounter = 0;

	glewExperimental = GL_TRUE;
	glewInit();

	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	return world.initialize();
}

void Application::handleEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			running = false;
		}
	}
}

void Application::update()
{
	updateCounter += 1;
	world.update();
}

void Application::render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	world.render();

	SDL_GL_SwapWindow(window);
}

void Application::cleanup()
{
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
