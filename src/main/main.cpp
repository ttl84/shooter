
#include "SDL2/SDL.h"

#include "state.h"
#include "keyboard/keyboard.h"
int main(int argc, char ** argv)
{
	float constexpr TIME_UNIT = 1.0 / 1000.0;
	float constexpr MAX_DT = 1.0 / 30.0;
	static State state;
	
	
	Timer timer;
	float dt = 0;
	while(! state.game.quit) {
		state.keyboard.update();

		dt += timer.dt();
		if(dt > MAX_DT)
			dt = MAX_DT;
		while(dt > TIME_UNIT) {
			state.update(TIME_UNIT);
			dt -= TIME_UNIT;
		}
		state.draw();
		
		timer.lap();
	}
	return 0;
}
