
#include "SDL2/SDL.h"

#include "GameState.h"
#include "input.h"

int main(int argc, char ** argv)
{
	float constexpr TIME_UNIT = 1.0 / 1000.0;
	float constexpr MAX_DT = 1.0 / 30.0;
	static GameState state("shooter game", 480, 480);
	
	
	
	Uint32 frame_begin = 0, frame_end = 0;
	float dt = TIME_UNIT;
	while(not control::quit)
	{
		frame_begin = frame_end;
		
		handle_event();
		if(dt > MAX_DT)
			dt = MAX_DT;
		while(dt > 0)
		{
			state.update(TIME_UNIT);
			dt -= TIME_UNIT;
		}
		state.draw();
		
		frame_end = SDL_GetTicks();
		dt += float(frame_end - frame_begin) / 1000.0;
	}
	return 0;
}
