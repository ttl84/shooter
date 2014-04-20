#include "GameState.h"
#include "debug.h"
#include "input.h"
#include "font.h"
#include "images.h"
#include <ctime>
#include <sstream>
GameState::GameState(std::string title, unsigned width, unsigned height):
	windowTitle(title),
	windowWidth(width),
	windowHeight(height)
{
	

	// sdl systems
	SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER);
	
	// video
	window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, 0);
	renderer = SDL_CreateRenderer(window, -1, 0);
	
	// input
	loadKeys();
	
	//font
	{
		bool good;
		std::tie(font, good) = loadFont("font.txt");
		for(auto pair : font)
		{
			this->fontTextureMap.emplace(pair.first, pair.second.makeTexture(renderer));
		}
	}
	
	// randomness
	{
		uint64_t seed;
		auto currentTime = time(nullptr);
		memcpy(&seed, &currentTime, std::min(sizeof currentTime, sizeof seed));
		seed ^= std::clock();

		PRG = std::mt19937(seed);
		starPRG = std::mt19937(seed);
		enemySpawnPRG = std::mt19937(seed);
	}
	
	stateReset();
}
GameState::~GameState()
{
	SDL_DestroyRenderer(renderer);
	renderer = nullptr;
	SDL_DestroyWindow(window);
	window = nullptr;
	
	SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER);
	SDL_Quit();
}
SDL_Texture* GameState::loadTexture(std::string filename)
{
	if(textureMap.find(filename) == textureMap.end())
	{
		Image image;
		bool good;
		std::tie(image, good) = loadImage(filename);
		
		SDL_Texture * texture;
		if(not good)
			texture = nullptr;
		else
			texture = image.makeTexture(renderer);
		if(texture == nullptr)
			debug::err << "GameState::loadTexture: unable to load [" << filename << ']' << std::endl;
		textureMap[filename] = texture;
	}
	return textureMap[filename];
}
SDL_Texture* GameState::loadFontTexture(char c)
{
	auto it = fontTextureMap.find(c);
	if(it == fontTextureMap.end())
		return nullptr;
	else
		return it->second;
}
Font const & GameState::getFont()
{
	return font;
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

void GameState:: enemyKill()
{
	score+= 3;
}
void GameState::enemyHit()
{
	score ++;
}
void GameState::playerDie()
{
	dead = true;
}
decltype(GameState::score) GameState::getScore() const
{
	return score;
}
void GameState::stateReset()
{
	// camera and existence boundary
	camera = Rect(0, 0, windowWidth, windowHeight);
	bounds.w = camera.w;
	bounds.h = camera.h * 2.0;
	bounds.setCenter(camera.getCenter());
	
	// progress tracking
	currentY = previousY = 0;
	
	// score keeping
	score = 0;

	// the player's state
	dead = false;
}
void drawText(GameState & state, std::string str, int x, int y)
{
	SDL_Rect dstrect;
	dstrect.x = x;
	dstrect.y = y;
	dstrect.w = state.getFont().getWidth();
	dstrect.h = state.getFont().getHeight();
	for(char c : str)
	{
		auto charTex = state.loadFontTexture(c);
		if(charTex != nullptr)
			SDL_RenderCopy(state.getRenderer(), charTex, nullptr, &dstrect);
		dstrect.x += dstrect.w + 1;
	}
}
std::tuple<unsigned, unsigned> textCenter(std::string str, Font const & font)
{
	unsigned x = str.length() * font.getWidth() / 2;
	unsigned y = font.getHeight() / 2;
	return std::make_tuple(x, y);
}
void GameState::drawUI()
{
	std::stringstream oss;
	oss << score;
	drawText(*this, oss.str(), 0, 0);
	
	if(dead)
	{
		std::string deadText("game over");
		unsigned x, y;
		std::tie(x, y) = textCenter(deadText, font);
		drawText(*this, deadText, windowWidth / 2 - x, windowHeight / 2 - y);
	}
}