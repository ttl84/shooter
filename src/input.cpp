#include "input.h"
#include "SDL2/SDL.h"
#include "filereader/FileReader.h"
#include <iostream>
#include <fstream>

static bool strToKeycode(std::string name, SDL_Keycode & output)
{
	#define BEGIN_CASE()\
	if(false){}
	
	
	
	#define MAP_CASE(str1, str2)\
	else if(name == str1)\
		output = SDLK_##str2
	
	#define GEN_CASE(str) MAP_CASE(#str, str)
	
	#define END_CASE()\
	else{\
		std::cerr << "unknown button " << name << std::endl;\
		return false;\
	}\
	return true;\
	
	BEGIN_CASE()
	MAP_CASE("up", UP);
	MAP_CASE("down", DOWN);
	MAP_CASE("left", LEFT);
	MAP_CASE("right", RIGHT);
	
	MAP_CASE("backspace", BACKSPACE);
	
	MAP_CASE("tab", TAB);
	MAP_CASE("\t", TAB);
	
	MAP_CASE("return", RETURN);
	MAP_CASE("enter", RETURN);
	
	MAP_CASE("space", SPACE);
	MAP_CASE(" ", SPACE);
	
	MAP_CASE("backslash", BACKSLASH);
	MAP_CASE("\\", BACKSLASH);
	
	MAP_CASE("quote", QUOTE);
	MAP_CASE("'", QUOTE);
	
	MAP_CASE("semicolon", SEMICOLON);
	MAP_CASE(";", SEMICOLON);
	
	MAP_CASE("<", COMMA);
	MAP_CASE(",", COMMA);
	
	MAP_CASE(">", PERIOD);
	MAP_CASE(".", PERIOD);
	
	GEN_CASE(a);
	GEN_CASE(b);
	GEN_CASE(c);
	GEN_CASE(d);
	GEN_CASE(e);
	GEN_CASE(f);
	GEN_CASE(g);
	GEN_CASE(h);
	GEN_CASE(i);
	GEN_CASE(j);
	GEN_CASE(k);
	GEN_CASE(l);
	GEN_CASE(m);
	GEN_CASE(n);
	GEN_CASE(o);
	GEN_CASE(p);
	GEN_CASE(q);
	GEN_CASE(r);
	GEN_CASE(s);
	GEN_CASE(t);
	GEN_CASE(u);
	GEN_CASE(v);
	GEN_CASE(w);
	GEN_CASE(x);
	GEN_CASE(y);
	GEN_CASE(z);
	
	GEN_CASE(0);
	GEN_CASE(1);
	GEN_CASE(2);
	GEN_CASE(3);
	GEN_CASE(4);
	GEN_CASE(5);
	GEN_CASE(6);
	GEN_CASE(7);
	GEN_CASE(8);
	GEN_CASE(9);
	
	END_CASE()
	
	#undef GEN_CASE
	#undef BEGIN_CASE
	#undef END_CASE
}
void loadKeyBinding(std::string filename, KeyBinding & keyBinding)
{
	std::ifstream is(filename);
	FileReader fr(is);
	
	auto obj = fr.getString("faster");
	if(obj != nullptr)
		strToKeycode(obj->datum, keyBinding.faster);
	
	obj = fr.getString("slower");
	if(obj != nullptr)
		strToKeycode(obj->datum, keyBinding.slower);
	
	obj = fr.getString("left");
	if(obj != nullptr)
		strToKeycode(obj->datum, keyBinding.left);
	
	obj = fr.getString("right");
	if(obj != nullptr)
		strToKeycode(obj->datum, keyBinding.right);
	
	obj = fr.getString("fire");
	if(obj != nullptr)
		strToKeycode(obj->datum, keyBinding.fire);
}

