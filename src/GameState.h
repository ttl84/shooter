#ifndef GameState_H
#define GameState_H
#include "SDL2/SDL.h"
#include "Rect.h"
#include "Vec2.h"
#include <string>
#include <random>
class GameState{
	float currentY;
	float previousY;
	float timeElapsed;

	SDL_Renderer * renderer;
	SDL_Window * window;
	
	std::mt19937 randomGenerator;
public:
	std::string const windowTitle;
	unsigned const windowWidth;
	unsigned const windowHeight;

	// objects overlapping with camera will be drawn
	Rect camera;

	// in general anything outside of bounds will be destroyed, and the edges are spawn points
	Rect bounds;

	GameState(std::string title, unsigned width, unsigned height);
	GameState(GameState const & other) = delete;
	GameState & operator = (GameState const & other) = delete;
	
	// set the center of the camera
	void centerCamera(Vec2 center);

	// set the center of the bounds
	void updateBounds(Vec2 center);

	// keeping track of progress
	void updateCurrentY(float);
	float getDistanceTravelled(void) const;
	float getTotalDistance(void) const;
			
	
	
	// initialize sdl2
	void init(void);
	
	// cleanup sdl2
	void cleanup(void);

	SDL_Renderer * getRenderer(void) { return renderer;}
	SDL_Window * getWindow(void) { return window;}
	std::mt19937 & getRandomGenerator(void) { return randomGenerator;}
	float randFloat(float low, float high)
	{
		return std::uniform_real_distribution<float>(low, high)(randomGenerator);
	}
	float randRoll(void)
	{
		return std::generate_canonical<float, 20>(randomGenerator);
	}
	
};
#endif
