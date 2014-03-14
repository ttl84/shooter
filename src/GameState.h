#ifndef GameState_H
#define GameState_H
#include "SDL2/SDL.h"
#include "Rect.h"
#include "Vec2.h"
#include <string>
class GameState{
	float currentY;
	float previousY;
	float timeElapsed;

	SDL_Renderer * renderer;
	SDL_Window * window;
public:
	std::string const windowTitle;
	unsigned const windowWidth;
	unsigned const windowHeight;
	Rect camera;
	Rect bounds;

	GameState(std::string title, unsigned width, unsigned height):
		windowTitle(title), windowWidth(width), windowHeight(height)
	{
		currentY = previousY = 0;
	}
	GameState(GameState const & other) = delete;
	
	void centerCamera(Vec2 center);
	void updateCurrentY(float);
	float getDistanceTravelled(void) const;
	float getTotalDistance(void) const;
			
	void updateBounds(Vec2 center);
	
	// initialize sdl2
	void init(void);
	
	// cleanup sdl2
	void cleanup(void);

	SDL_Renderer * getRenderer(void) { return renderer;}
	SDL_Window * getWindow(void) { return window;}
	
};
#endif
