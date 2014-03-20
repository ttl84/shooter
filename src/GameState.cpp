#include "GameState.h"
#include "debug.h"
GameState::GameState(std::string title, unsigned width, unsigned height):
	windowTitle(title),
	windowWidth(width),
	windowHeight(height)
{
	currentY = previousY = 0;
	std::random_device rd;
	auto seed = rd();
	debug::log << "seed is " << seed << std::endl;
	randomGenerator = std::mt19937(seed);
}
void GameState::init(void)
{	
	SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER);
	
	window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, 0);
	renderer = SDL_CreateRenderer(window, -1, 0);
	
	camera = Rect(0, 0, windowWidth, windowHeight);
	bounds.w = camera.w;
	bounds.h = camera.h * 2.0;
	bounds.setCenter(camera.getCenter());
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
void GameState::centerCamera(Vec2 center)
{
	camera.setCenter(center);
}
void GameState::updateBounds(Vec2 center)
{
	bounds.setCenterY(center.y);
}
void GameState::updateCurrentY(float newY)
{
	previousY = currentY;
	currentY = newY;
}
float GameState :: getDistanceTravelled(void) const
{
	return -(currentY - previousY);
}
float GameState :: getTotalDistance(void) const
{
	return -currentY;
}