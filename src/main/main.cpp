#include "state.h"
#include "util/timer.h"
int main(int argc, char ** argv)
{
	double constexpr TIME_UNIT = 1.0 / 1000.0;
	double constexpr MAX_DT = 1.0 / 30.0;
	static State state;
	
	
	util::Timer timer;
	double dt = TIME_UNIT;
	while(! state.game.quit) {
		state.keyboard.update();
		if(dt > MAX_DT)
			dt = MAX_DT;
		while(dt >= TIME_UNIT) {
			state.update(TIME_UNIT);
			dt -= TIME_UNIT;
		}
		state.draw();
		
		timer.lap();
	}
	return 0;
}
