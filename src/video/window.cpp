#include "window.h"
#include "Font.h"
#include "SDL2/SDL_image.h"
#include <iostream>
Window::Window()
	:renderer(nullptr), window(nullptr)
{

}

Window::Window(char const * name, unsigned w, unsigned h)
	:title(name), width(w), height(h)
{
	window = SDL_CreateWindow(
				title.c_str(),
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED,
				width,
				height,
				0);
	if(window == nullptr) {
		std::cerr << "sdl window error: " << SDL_GetError() << "\n";
		return;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if(renderer != nullptr) {
		SDL_DestroyWindow(data->window);
		std::cerr << "sdl renderer error: " << SDL_GetError() << "\n";
		return;
	}
}

Window::~Window()
{
	this->destroy();
}

void Window::destroy()
{
	for(auto p : textureMap) {
		SDL_DestroyTexture(p.second);
	}
	for(auto p : fontTextureMap) {
		SDL_DestroyTexture(p.second);
	}
	if(renderer) {
		SDL_DestroyRenderer(renderer);
		renderer = nullptr;
	}
	if(window) {
		SDL_DestroyWindow(window);
		window = nullptr;
	}
}

bool Window::loadFont(char const * path)
{
	Font font;
	bool good;
	std::tie(font, good) = loadFont(path);
	if(good)
		for(auto p : font)
			fontTextureMap.emplace(
						p.first,
						p.second.makeTexture(data->renderer));
	return good;
}

SDL_Texture * Window::loadTexture(char const * path)
{
	auto it = textureMap.find(name);
	if(it != textureMap.end()) {
		return it->second;
	}

	SDL_Texture * texture = 0;
	SDL_Surface * surface = IMG_Load(name);
	if(surface != 0) {
		texture = SDL_CreateTextureFromSurface(
			data, surface);
		SDL_FreeSurface(surface);
	}
	if(texture != 0) {
		textureMap.emplace(name, texture);
	}
	return texture;
}
