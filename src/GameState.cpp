#include "GameState.h"
#include "debug.h"
#include "input.h"
#include "font.h"
#include "images.h"
#include "systems.h"
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

	entities.reset();
	entities.scheduleCreationJob(spawnPlayerFunc(entities, *this));
	entities.executeCreationJobs();
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

using ecs::Component;
namespace player_stat{
	float fast_speed = 200;
	float slow_speed = 50;
	float normal_speed = slow_speed + (fast_speed - slow_speed) / 2.0;
	float forward_accel = 150;
	float backward_accel = 50;
	float side_accel = 1000;
}
void keyboard_control(ecs::Entity & entities, ecs::entity_t i)
{
	float speed = entities.velocity[i].norm();
	
	// makes sure player  doesn't move backwards
	if(fabs(entities.velocity[i].y) > player_stat::fast_speed)
		entities.velocity[i].y = player_stat::fast_speed * -1;
	if(fabs(entities.velocity[i].y) < player_stat::slow_speed)
		entities.velocity[i].y = player_stat::slow_speed * -1;
	
	
	if(control::faster)
	{
		if(speed < player_stat::fast_speed)
			entities.acceleration[i].y = -player_stat::forward_accel;
		else
			entities.acceleration[i].y = 0;
	}
	else if(control::slower)
	{
		if(speed > player_stat::slow_speed)
			entities.acceleration[i].y = player_stat::backward_accel;
		else
			entities.acceleration[i].y = 0;
	}
	else
	{
		entities.acceleration[i].y = 0;
	}
	
	if(control::left)
	{
		entities.acceleration[i].x = -player_stat::side_accel;
	}
	else if(control::right)
	{
		entities.acceleration[i].x = player_stat::side_accel;
	}
	else
	{
		entities.acceleration[i].x = -2 * entities.velocity[i].x;
	}
	
	float forwardAngle = Vec2(0, -1).angle();
	float velocityAngle = entities.velocity[i].angle();
	float diffAngle = velocityAngle -  forwardAngle;
	entities.direction[i] = forwardAngle + diffAngle / 4;
	if(control::fire)
	{
		entities.gun[i].fire = true;
	}
	else
	{
		entities.gun[i].fire = false;
	}
	
}
ecs::entity_t get_player(ecs::Entity & entities)
{
	ecs::entity_t i;
	for(i = 0; i < entities.count(); i++)
		if(ecs::accepts<Component::CAMERA_FOCUS>(entities.mask[i]))
			break;
	return i;
}
Gun player_gun(ecs::Entity & entities, GameState & state)
{
	Gun g;
	g.delay = 0.1;
	g.bullet_speed = 550.0;
	g.wait_time = 0;
	
	
	
	
	g.gun_function = [&entities, &state](ecs::entity_t gunner)
	{
		
		std::function<void(ecs::Entity &, unsigned)> bulletCreationFunc =
		[gunner, &state](ecs::Entity & e, unsigned bullet){
			e.mask[bullet] = ecs::bullet_mask;
			
			e.image[bullet] = state.loadTexture("bullet0");
			
			e.position[bullet] = e.position[gunner];
			e.velocity[bullet] = e.velocity[gunner] + e.gun[gunner].bullet_speed * 
				Vec2::fromAngle(e.direction[gunner] + state.randFloat(-0.01, 0.01));
			e.direction[bullet] = e.direction[gunner];
			
			e.collision_effect[bullet] = [&](ecs::entity_t victim){
				e.health[victim] -= 1;
				state.enemyHit();
			};
			e.faction[bullet] = Faction::PLAYER;
			
			e.health[bullet] = 2;
			
			// visual effect for bullet if it hits something
			e.death_function[bullet] = [&e, &state](ecs::entity_t bullet) -> void{
				e.timer[bullet] = 0.5;
				e.image[bullet] = state.loadTexture("explosion0");
				e.mask[bullet] = ecs::combine(
					Component::TIMER,
					Component::IMAGE,
					Component::POSITION);
				
			};
			
			e.timer[bullet] = 2;
			e.timer_function[bullet] = [&e](ecs::entity_t gunner){e.remove(gunner);};
		};
		entities.scheduleCreationJob(bulletCreationFunc);
	};
	return g;
}
Gun basic_gun(ecs::Entity & entities, GameState & state)
{
	Gun g;
	g.delay = state.randFloat(0.2, 1.0);
	g.bullet_speed = state.randFloat(50.0, 150.0);
	g.wait_time = 0;
	g.gun_function = [&entities, &state](ecs::entity_t gunner)
	{
		std::function<void(ecs::Entity &, unsigned)> bulletCreationFunc =
		[gunner, &state](ecs::Entity & e, unsigned bullet){
			e.mask[bullet] = ecs::bullet_mask;
			
			e.position[bullet] = e.position[gunner];
			
			ecs::entity_t target = e.target[gunner];
			Vec2 direction = e.position[target] - e.position[gunner];
			if(direction.norm() > 0)
				direction /= direction.norm();
			e.velocity[bullet] = e.velocity[gunner] + e.gun[gunner].bullet_speed * direction;
			e.image[bullet] = state.loadTexture("bullet1");
			e.timer[bullet] = 4.0;
			e.timer_function[bullet] = [&e](ecs::entity_t bullet){
				e.health[bullet] = 0;
			};
			
			e.health[bullet] = 1;
			e.death_function[bullet] = [&e](ecs::entity_t bullet){
				e.remove(bullet);
			};
			
			e.collision_effect[bullet] = [&](ecs::entity_t victim){
				e.health[victim] -= 1;
			};
			e.faction[bullet] = e.faction[gunner];
		};
		entities.scheduleCreationJob(bulletCreationFunc);
	};
	g.fire = true;
	return g;
}
std::function<void(ecs::Entity &, unsigned)> 
spawnPlayerFunc(ecs::Entity & entities, GameState & state)
{
	std::function<void(ecs::Entity &, unsigned)> playerCreationFunc =
		[&state](ecs::Entity & e, unsigned player){
	
		e.mask[player] = ecs::player_mask;
		
		e.image[player] = state.loadTexture("player");
		
		{
			int w, h;
			SDL_QueryTexture(e.image[player], nullptr, nullptr, &w, &h);
			e.size[player] = Size(w, h);
		}
		e.position[player] = state.camera.getCenter();
		
		e.velocity[player] = player_stat::normal_speed * Vec2(0, -1);
		e.acceleration[player] = Vec2(0, 0);
		e.direction[player] = Vec2(0, -1).angle();
		
		
		e.gun[player] = player_gun(e, state);
		
		e.health[player] = 3;
		
		e.collision_effect[player] = [&](ecs::entity_t victim){
			e.health[victim] -= 10;
		};
		
		e.faction[player] = Faction::PLAYER;
		
		e.think_function[player] = [&e](ecs::entity_t player){
			keyboard_control(e, player);
		};
		
		e.death_function[player] = [&](ecs::entity_t player){
			e.mask[player] = combine(ecs::move_mask, Component::CAMERA_FOCUS);
			e.position[player].x = state.camera.getCenter().x;
			e.velocity[player].x = 0;
			state.playerDie();
		};
	};
	return playerCreationFunc;
}
std::function<void(ecs::Entity &, unsigned)> 
spawnEnemyFunc1(ecs::Entity & entities, GameState & state)
{
	std::function<void(ecs::Entity &, unsigned)> enemyCreationFunc =
		[&state](ecs::Entity & e, unsigned enemy){
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
		
		
		e.gun[enemy] = basic_gun(e, state);
		
		e.health[enemy] = 3;
		
		e.collision_effect[enemy] = [&e](ecs::entity_t victim){
			e.health[victim] -= 2;
		};
		
		e.faction[enemy] = Faction::ENEMY;
			
		e.target[enemy] = get_player(e);
		
		e.death_function[enemy] = [&e, &state](ecs::entity_t enemy){
			e.mask[enemy] = ecs::combine(
				Component::IMAGE,
				Component::TIMER,
				Component::POSITION);
			
			e.image[enemy] = state.loadTexture("explosion2");
			
			
			e.timer[enemy] = 0.5;
			e.timer_function[enemy] = [&e](ecs::entity_t enemy){
				e.remove(enemy);
			};
			
			state.enemyKill();
		};
	};
	return enemyCreationFunc;
}

std::function<void(ecs::Entity &, unsigned)> 
spawnEnemyFunc2(ecs::Entity & entities, GameState & state)
{
	auto enemyCreationFunc1 = spawnEnemyFunc1(entities, state);
	std::function<void(ecs::Entity &, unsigned)> enemyCreationFunc2 =
		[&state, enemyCreationFunc1](ecs::Entity & e, unsigned enemy){
		enemyCreationFunc1(e, enemy);
		e.position[enemy].y = state.bounds.getBottom();
		e.velocity[enemy] = Vec2(0, -player_stat::fast_speed);
		e.direction[enemy] = Vec2(0, -1).angle();
		
		e.image[enemy] = state.loadTexture("enemy0");
		
		e.mask[enemy] |= ecs::combine(Component::THINK, Component::ACCELERATION, Component::TIMER);
		e.think_function[enemy] = [&e](ecs::entity_t enemy){
			ecs::entity_t target = e.target[enemy];
			e.velocity[enemy].y = e.velocity[target].y + (-50);
			e.mask[enemy] &= (~ecs::combine(Component::THINK));
		};
		e.timer[enemy] = 10.0;
		e.timer_function[enemy] = [&e, &state](ecs::entity_t i){
			e.think_function[i] = [&e, &state](ecs::entity_t i){
				e.acceleration[i] = Vec2(state.randFloat(-50, 50), -50);
			};
		};
	};
	return enemyCreationFunc2;
}



void spawn_enemies(ecs::Entity & entities, GameState & state)
{
	auto roll = state.randEnemySpawn();
	if(roll <= 0.0001)
		entities.scheduleCreationJob(spawnEnemyFunc2(entities, state));
	else if(roll <= 0.0002)
		entities.scheduleCreationJob(spawnEnemyFunc1(entities, state));
}
void update_camera(ecs::Entity & entities, GameState & state)
{
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		if(ecs::accepts<Component::CAMERA_FOCUS>(entities.mask[i]))
		{
			float center_y = entities.position[i].y;
			state.updateCurrentY(center_y);
			float adjustment = entities.velocity[i].y + player_stat::normal_speed;
			float multiplier = state.windowHeight * 0.95 / (player_stat::fast_speed - player_stat::slow_speed);
			state.camera.setCenterY(floor(center_y - adjustment * multiplier));
			return;
		}
	}
			
}
void update_bounds(ecs::Entity & entities, GameState & state)
{
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		if(ecs::accepts<Component::CAMERA_FOCUS>(entities.mask[i]))
		{
			Vec2 center = entities.position[i];
			state.updateBounds(center);
			return;
		}
	}
			
}
void despawn_entities(ecs::Entity & entities, GameState & state)
{
	for(ecs::entity_t i = 0; i < entities.count(); i++)
	{
		if(ecs::accepts<Component::POSITION>(entities.mask[i]))
		{
			if(not state.bounds.contains(entities.position[i]))
			{
				entities.remove(i);
			}
		}
	}
}

void GameState::update(float const dt)
{
	thinkSystem(entities);
	shootSystem(entities, dt);
	
	accelSystem(entities, dt);
	moveSystem(entities, dt);
	
	collisionSystem(entities);
	timerSystem(entities, dt);
	deathSystem(entities);
	
	update_camera(entities, *this);
	update_bounds(entities, *this);
	
	spawn_enemies(entities, *this);
	despawn_entities(entities, *this);

	updateStars();
	
	entities.executeCreationJobs();
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