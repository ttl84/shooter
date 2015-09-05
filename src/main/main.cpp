#include "state.h"
#include "util/timer.h"
int main(int argc, char ** argv)
{
	double constexpr TIME_UNIT = 1.0 / 1000.0;

	// minimum and maximum number of game updates per frame
	unsigned constexpr MIN_UPDATES = 1;
	unsigned constexpr MAX_UPDATES = 30;
	// the current desired number of updates per frame
	unsigned update_goal = MIN_UPDATES;
	// time earned or lost from updating
	double time_balance = 0;

	static State state;
	state.io.window = state.io.video.makeWindow("shooter", 640, 640);
	
	util::Timer timer;
	double dt = TIME_UNIT;
	while(! state.game.quit) {
		state.io.keyboard.update();

		unsigned updates_left = update_goal;
		while(dt >= TIME_UNIT && updates_left != 0) {
			state.update(TIME_UNIT);
			dt -= TIME_UNIT;
			updates_left--;
		}
		// depending on how the previous loop finished,
		// update_goal will be adjusted up or down, to a limit
		if(dt <= 0 && update_goal <= MAX_UPDATES) {
			time_balance += TIME_UNIT * updates_left;
			update_goal++;
		} else if(updates_left == 0 && update_goal > MIN_UPDATES) {
			time_balance -= dt;
			dt = 0;
			update_goal--;
		}

		state.draw();
		
		dt += timer.lap();
	}
	return 0;
}
