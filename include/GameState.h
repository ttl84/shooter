#ifndef GameState_H
#define GameState_H
#include "SDL2/SDL.h"
#include "Rect.h"
#include "Vec2.h"
#include "Font.h"
#include "Entity.h"
#include <deque>
#include <string>
#include <random>
#include <unordered_map>
class GameState{
private:
	float currentY;
	float previousY;
	float timeElapsed;
	uint64_t score;
	bool dead;

	ecs::Entity entities;

	std::deque<Vec2> stars;

	std::mt19937 PRG, starPRG, enemySpawnPRG;

	SDL_Renderer * renderer;
	SDL_Window * window;
	
	Font font;
	std::unordered_map<char, SDL_Texture*> fontTextureMap;
	std::unordered_map<std::string, SDL_Texture*> textureMap;
	
	
	
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
	float getDistanceTravelled() const;
	float getTotalDistance() const;
	decltype(score) getScore() const;

	void enemyKill();
	void enemyHit();
	void playerDie();

	
	
	void reset();
	
	GameState(std::string title, unsigned width, unsigned height);
	~GameState();
	GameState(GameState const & other) = delete;
	GameState & operator = (GameState const & other) = delete;

	SDL_Texture * loadTexture(std::string name);
	SDL_Texture * loadFontTexture(char c);
	Font const & getFont();
	
	SDL_Renderer * getRenderer()
	{
		return renderer;
	}
	SDL_Window * getWindow()
	{
		return window;
	}
	float randFloat(float low, float high)
	{
		return std::uniform_real_distribution<float>(low, high)(PRG);
	}
	float randEnemySpawn()
	{
		return std::generate_canonical<float, 32>(enemySpawnPRG);
	}

	void drawUI();
	void updateStars();
	void drawStars();
	void update(float dt);
	void draw();
	
};
std::function<void(ecs::Entity &, unsigned)> 
spawnPlayerFunc(ecs::Entity & entities, GameState & state);

std::function<void(ecs::Entity &, unsigned)> 
spawnEnemyFunc1(ecs::Entity & entities, GameState & state);

std::function<void(ecs::Entity &, unsigned)> 
spawnEnemyFunc2(ecs::Entity & entities, GameState & state);

Gun player_gun(ecs::Entity & entities, GameState & state);
Gun basic_gun(ecs::Entity & entities, GameState & state);
#endif
