#include "GameState.h"

void GameState::init(void)
{	
	SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER);
	
	window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, 0);
	renderer = SDL_CreateRenderer(window, -1, 0);
	
	camera = Rect(0, 0, windowWidth, windowHeight);
}
void GameState::cleanup(void)
{
	SDL_DestroyRenderer(renderer);
	renderer = nullptr;
	SDL_DestroyWindow(window);
	window = nullptr;
	
	SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER);
	SDL_Quit();
}
void GameState::setStart(float start)
{
	startY = start;
	previousY = start;
	currentY = start;
}
void GameState::updateCurrent(float newY)
{
	previousY = currentY;
	currentY = newY;
}