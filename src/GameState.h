#ifndef GameState_H
#define GameState_H
#include "SDL2/SDL.h"
#include "Rect.h"
#include "Vec2.h"
#include <string>
class GameState{
	float currentY;
	float previousY;
	float startY;
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
		windowTitle(title), windowWidth(width), windowHeight(height){}
	GameState(GameState const & other) = delete;
			
	void setStart(float);
	void updateCurrent(float);
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
