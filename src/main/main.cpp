#include "state.h"
#include "util/timer.h"
int main(int argc, char ** argv)
{
	double constexpr TIME_UNIT = 1.0 / 1000.0;
	unsigned constexpr MAX_UPDATES = 25;
	static State state;
	
	
	util::Timer timer;
	double dt = TIME_UNIT;
	while(! state.game.quit) {
		state.keyboard.update();

		unsigned updates = MAX_UPDATES;
		while(dt >= TIME_UNIT && updates != 0) {
			state.update(TIME_UNIT);
			dt -= TIME_UNIT;
			updates--;
		}
		state.draw();
		
		dt += timer.lap();
	}
	return 0;
}
