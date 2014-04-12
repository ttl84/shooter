#ifndef GameState_H
#define GameState_H
#include "SDL2/SDL.h"
#include "Rect.h"
#include "Vec2.h"
#include <string>
#include <random>
#include <unordered_map>
class GameState{
	float currentY;
	float previousY;
	float timeElapsed;

	SDL_Renderer * renderer;
	SDL_Window * window;
	
	std::unordered_map<std::string, SDL_Texture*> textureMap;
	
	std::mt19937 randomGenerator;
public:
	std::string const windowTitle;
	unsigned const windowWidth;
	unsigned const windowHeight;

	// objects overlapping with camera will be drawn
	Rect camera;

	// anything outside of bounds will be destroyed, and the edges are spawn points
	Rect bounds;

	
	
	// set the center of the camera
	void centerCamera(Vec2 center);

	// set the center of the bounds
	void updateBounds(Vec2 center);

	// keeping track of progress
	void updateCurrentY(float);
	float getDistanceTravelled(void) const;
	float getTotalDistance(void) const;
			
	
	
	GameState(std::string title, unsigned width, unsigned height);
	~GameState();
	GameState(GameState const & other) = delete;
	GameState & operator = (GameState const & other) = delete;

	SDL_Texture * loadTexture(std::string name);
	SDL_Renderer * getRenderer()
	{
		return renderer;
	}
	SDL_Window * getWindow()
	{
		return window;
	}
	std::mt19937 & getRandomGenerator()
	{
		return randomGenerator;
	}
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
