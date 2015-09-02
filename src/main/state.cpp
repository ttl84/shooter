#include "state.h"
#include "util/debug.h"
#include "keyboard/keyboard.h"
#include "video/Font.h"
#include "video/images.h"
#include "stats.h"
#include <ctime>
#include <sstream>

/*
Gun player_gun(GameState & state)
{
	Gun g;
	g.delay = 0.1;
	g.bullet_speed = 550.0;
	g.wait_time = 0;
	
	g.gun_function = [&state](unsigned gunner)
	{
		
		state.schedule([gunner, &state](ecs::Entity & e){
			unsigned bullet = e.claim();
			e.mask[bullet] = ecs::bullet_mask;
			
			e.image[bullet] = state.loadTexture("bullet0");
			
			e.position[bullet] = e.position[gunner];
			e.velocity[bullet] = e.velocity[gunner] + e.gun[gunner].bullet_speed * 
				Vec2::fromAngle(e.direction[gunner] + state.randFloat(-0.01, 0.01));
			e.direction[bullet] = e.direction[gunner];
			
			e.collision_effect[bullet] = [&](unsigned victim){
				e.life[victim].value -= 1;
				state.enemyHit();
			};
			e.faction[bullet] = Faction::PLAYER;
			
			// bullet lives for two seconds
			e.timer[bullet].remaining = 2;
			e.timer[bullet].action = [&e](unsigned gunner){
				e.remove(gunner);
			};
			
			// visual effect happens when bullet dies
			e.life[bullet].value = 2;
			e.life[bullet].deathAction = [&e, &state](unsigned bullet) -> void{
				e.timer[bullet].remaining = 0.5;
				e.image[bullet] = state.loadTexture("explosion0");
				e.mask[bullet] = ecs::combine(
					ecs::Component::TIMER,
					ecs::Component::IMAGE,
					ecs::Component::POSITION);
				
			};
			
			// play sound
			auto sound = state.loadSound("sound/laser.wav");
			state.playSound(sound);
		});
	};
	return g;
}
Gun basic_gun(GameState & state)
{
	Gun g;
	g.delay = state.randFloat(0.2, 1.0);
	g.bullet_speed = state.randFloat(50.0, 150.0);
	g.wait_time = 0;
	g.gun_function = [&state](unsigned gunner)
	{
		state.schedule([gunner, &state](ecs::Entity & e){
			unsigned bullet = e.claim();
			e.mask[bullet] = ecs::bullet_mask;
			
			e.position[bullet] = e.position[gunner];
			
			unsigned target = e.target[gunner];
			Vec2 direction = e.position[target] - e.position[gunner];
			if(direction.norm() > 0)
				direction /= direction.norm();
			e.velocity[bullet] = e.velocity[gunner] + e.gun[gunner].bullet_speed * direction;
			e.image[bullet] = state.loadTexture("bullet1");

			e.timer[bullet]= {4.0, [&e](unsigned bullet){
				e.life[bullet].value = 0;
			}};
			
			e.life[bullet].value = 1;
			e.life[bullet].deathAction = [&e](unsigned bullet){
				e.remove(bullet);
			};
			
			e.collision_effect[bullet] = [&](unsigned victim){
				e.life[victim].value -= 1;
				auto sound = state.loadSound("sound/hit.wav");
				state.playSound(sound);
			};
			e.faction[bullet] = e.faction[gunner];
		});
	};
	g.fire = true;
	return g;
}
*/
// cap ship values to its physical limits
void ship_constraint(State & state, unsigned i)
{
	Entity & e = state.getEntities();
	ShipStats const * stats = e.shipstats[i];

	float speed = e.velocity[i].norm();
	if(speed > stats->max_speed)
		e.velocity[i] *= (stats->max_speed / speed);
	else if(speed < stats.min_speed)
		e.velocity[i] *= (stats->min_speed / speed);

	float speed_ang = e.speed_ang[i];
	if(speed_ang > stats->max_speed_ang)
		e.speed_ang[i] = stats->max_speed_ang;
	else if(speed_ang < stats->min_speed_ang)
		e.speed_ang[i] = stats->min_speed_ang;
}

// does what the ship wants
void ship_control(GameState & state, unsigned i)
{
	Entity & e = state.getEntities();
	ShipBrain const & brain = e.ship_brain[i];
	ShipStats const * stats = e.ship_stats[i];

	
	if(brain.accel) {
		Vec2 accel(0, -stats->.accel);
		accel.rotate(e.direction[i]);
		e.accel[i] = accel;
	} else {
		e.accel[i] = Vec2(0,0);
	}

	e.accel_ang[i] = 0;
	if(brain.turn_right)
		e.accel_ang[i] += stats->accel_ang; 
	if(brain.turn_left)
		e.accel_ang[i] -= stats->accel_ang;

	
		
}
// use keyboard inputs to change brain state
void keyboard_think(GameState & state, unsigned i)
{
	auto & e = state.getEntities();
	auto & keyPress = state.getKeyPress();
	ShipBrain & brain = e.ship_brain[i];

	brain.accel = keyPress.faster;
	brain.turn_left = keyPress.left;
	brain.turn_right = keyPress.right;
	brain.shoot = keyPress.fire;
}

void GameState::initPRG()
{
	uint64_t seed;
	auto currentTime = time(nullptr);
	memcpy(&seed, &currentTime, std::min(sizeof currentTime, sizeof seed));
	seed ^= std::clock();

	PRG = std::mt19937(seed);
	starPRG = std::mt19937(seed);
	enemySpawnPRG = std::mt19937(seed);
}
void GameState::reset()
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

	entities.reset();
	schedule([this](ecs::Entity & e){
		unsigned player = e.claim();
		e.mask[player] = ecs::player_mask;
		
		e.image[player] = this->loadTexture("player");
		
		{
			int w, h;
			SDL_QueryTexture(e.image[player], nullptr, nullptr, &w, &h);
			e.size[player] = Size(w, h);
		}
		e.position[player] = this->camera.getCenter();
		
		e.velocity[player] = player_stat::normal_speed * Vec2(0, -1);
		e.acceleration[player] = Vec2(0, 0);
		e.direction[player] = Vec2(0, -1).angle();
		
		
		e.gun[player] = player_gun(*this);
		
		
		
		e.collision_effect[player] = [&](unsigned victim){
			e.life[victim].value -= 10;
		};
		
		e.faction[player] = Faction::PLAYER;
		
		e.think_function[player] = [this](unsigned player){
			keyboard_control(*this, player);
		};
		
		e.life[player].value = 3;
		e.life[player].deathAction = [&](unsigned player){
			e.mask[player] = combine(ecs::move_mask, ecs::Component::CAMERA_FOCUS);
			e.position[player].x = this->camera.getCenter().x;
			e.velocity[player].x = 0;
			this->playerDie();
		};
	});
	execute();
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
	{
		std::stringstream oss;
		oss << "score: " << score;
		drawText(*this, oss.str(), 0, 0);
	}
	{
		std::stringstream oss;
		oss << "frame time: " << this->dt;
		drawText(*this, oss.str(), 0, font.getHeight());
	}
	if(dead)
	{
		std::string deadText("game over");
		std::string retryText("press any key");
		unsigned x, y;

		std::tie(x, y) = textCenter(deadText, font);
		drawText(*this, deadText, windowWidth / 2 - x, windowHeight / 2 - y);

		std::tie(x, y) = textCenter(retryText, font);
		drawText(*this, retryText, windowWidth / 2 - x, windowHeight / 2 + font.getHeight() + 2 - y);
	}
}

void GameState::updateStars()
{
	float roll = std::generate_canonical<float, 20>(starPRG);
	if(roll < 0.01)
	{
		auto distr = std::uniform_real_distribution<float>(camera.getLeft(), camera.getRight());
		float randomX = distr(starPRG);
		stars.emplace_back(randomX, bounds.getTop());
	}

	while((not stars.empty()) and (stars.front().y > camera.getBottom()))
	{
		stars.pop_front();
	}
}
void GameState::drawStars()
{
	SDL_Texture * star_tex = loadTexture("star");
	int width, height;
	SDL_QueryTexture(star_tex, nullptr, nullptr, &width, &height);
	for(Vec2 & star : stars)
	{
		SDL_Rect position;
		position.x = star.x - camera.x;
		position.y = star.y - camera.y;
		position.w = width;
		position.h = height;
		SDL_RenderCopy(renderer, star_tex, nullptr, &position);
	}
}




unsigned get_player(ecs::Entity & entities)
{
	unsigned i;
	for(i = 0; i < entities.count(); i++)
		if(ecs::accepts<ecs::Component::CAMERA_FOCUS>(entities.mask[i]))
			break;
	return i;
}


std::function<void(ecs::Entity &)> 
spawnEnemyFunc1(GameState & state)
{
	return [&state](ecs::Entity & e){
		unsigned enemy = e.claim();
		e.mask[enemy] = ecs::enemy_mask;
		
		e.image[enemy] = state.loadTexture("enemy0");
		
		{
			int w, h;
			SDL_QueryTexture(e.image[enemy], nullptr, nullptr, &w, &h);
			e.size[enemy] = Size(w, h);
		}
		{
			float randomX = state.randFloat(state.camera.getLeft(), state.camera.getRight());
			e.position[enemy] = Vec2(randomX, state.bounds.getTop());
		}
		{
			float randomSpeed = state.randFloat(25.0, 70.0);
			e.velocity[enemy] = Vec2(0, randomSpeed);
		}
		e.direction[enemy] = Vec2(0, 1).angle();
		
		
		e.gun[enemy] = basic_gun(state);
		
		
		
		e.collision_effect[enemy] = [&e](unsigned victim){
			e.life[victim].value -= 2;
		};
		
		e.faction[enemy] = Faction::ENEMY;
			
		e.target[enemy] = get_player(e);
		
		e.life[enemy].value = 3;
		e.life[enemy].deathAction = [&e, &state](unsigned enemy){
			e.mask[enemy] = ecs::combine(
				ecs::Component::IMAGE,
				ecs::Component::TIMER,
				ecs::Component::POSITION);
			
			e.image[enemy] = state.loadTexture("explosion2");
			
			e.timer[enemy] = {0.5, [&e](unsigned enemy){
				e.remove(enemy);
			}};
			
			state.enemyKill();
		};
	};
}


void spawn_enemies(GameState & state)
{
	auto roll = state.randEnemySpawn();
	if(roll <= 0.002)
		state.schedule(spawnEnemyFunc1(state));
}
void update_camera(GameState & state)
{
	auto & entities = state.getEntities();
	for(unsigned i = 0; i < entities.count(); i++)
	{
		if(ecs::accepts<ecs::Component::CAMERA_FOCUS>(entities.mask[i]))
		{
			float center_y = entities.position[i].y;
			state.updateCurrentY(center_y);
			float adjustment = entities.velocity[i].y + player_stat::normal_speed;
			float multiplier = state.windowHeight * 0.65 / (player_stat::fast_speed - player_stat::slow_speed);
			state.camera.setCenterY(floor(center_y - adjustment * multiplier));
			return;
		}
	}
			
}
void update_bounds(GameState & state)
{
	auto & entities = state.getEntities();
	for(unsigned i = 0; i < entities.count(); i++)
	{
		if(ecs::accepts<ecs::Component::CAMERA_FOCUS>(entities.mask[i]))
		{
			Vec2 center = entities.position[i];
			state.updateBounds(center);
			return;
		}
	}
			
}
void trap_player(GameState & state)
{
	auto & entities = state.getEntities();
	for(unsigned i = 0; i < entities.count(); i++)
	{
		if(ecs::accepts<ecs::Component::CAMERA_FOCUS>(entities.mask[i]))
		{
			float centerX = entities.position[i].x;
			float low = state.bounds.getLeft();
			float high = state.bounds.getRight();
			if(centerX <= low)
				centerX = low + 1;
			else if(centerX >= high)
				centerX = high - 1;
			entities.position[i].x = centerX;
			return;
		}
	}
			
}
void despawn_entities(GameState & state)
{
	auto & entities = state.getEntities();
	for(unsigned i = 0; i < entities.count(); i++)
	{
		if(ecs::accepts<ecs::Component::POSITION>(entities.mask[i]))
		{
			if(not state.bounds.contains(entities.position[i]))
			{
				entities.remove(i);
			}
		}
	}
}

void State::update(double const dt)
{
	world.update(dt);
}



void GameState::draw()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	
	drawStars();
	drawSystem(entities, renderer, camera);
	drawUI();
	SDL_RenderPresent(renderer);
}

