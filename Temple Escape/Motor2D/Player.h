#ifndef __j1Player_H__
#define __j1Player_H__

#include "p2Animation.h"
#include "p2Point.h"
#include "Entity.h"

#define PLAYER_SPEED 40.0f
#define JUMP_SPEED 40.0f
#define GRAVITY 60.0f

struct SDL_Texture;
struct Collider;

static fPoint tmp;

class Player : public Entity
{

	enum PlayerState
	{
		ST_UNKNOWN,

		ST_IDLE_RIGHT,
		ST_IDLE_LEFT,
		ST_RUN_RIGHT,
		ST_RUN_LEFT,
		ST_JUMP_RIGHT,
		ST_JUMP_LEFT,
		ST_SHOOT_RIGHT,
		ST_SHOOT_LEFT
	};

	enum PlayerLastState
	{
		LAST_ST_UNKNOWN,

		LAST_ST_IDLE_RIGHT,
		LAST_ST_IDLE_LEFT,
		LAST_ST_RUN_RIGHT,
		LAST_ST_RUN_LEFT,
		LAST_ST_JUMP_RIGHT,
		LAST_ST_JUMP_LEFT,
		LAST_ST_SHOOT_RIGHT,
		LAST_ST_SHOOT_LEFT
	};

	enum Direction {
		RIGHT,
		LEFT,
		UP,
		DOWN,

		NO_DIR
	};

private:
	bool didDoubleJump;
	bool isJumping;

	uint currentTime;
	uint jumpTimer;

	float player_speed;
	float current_dt;

	iPoint last_save_position;

private:
	float gravityHaveToWork();
	float canGoRight();
	float canGoLeft();
	float canGoUp();
	float DistanceToWall(SDL_Rect wall, SDL_Rect player, Direction direction);

public:
	Player(int x, int y);
	~Player();

	void Update(float dt);

	void SetEntitiesSpeed(float dt);

	void OnCollision(Collider* collider);

	bool Load(pugi::xml_node&);

	void ImplementLoad();

	bool Save(pugi::xml_node&) const;

public:
	PlayerState current_state;
	SDL_Rect current_frame;
	PlayerLastState last_state;

	uint right_idle_vel, left_idle_vel, right_run_vel, left_run_vel, right_jump_vel,
		left_jump_vel, right_death_blink_vel, left_death_blink_vel, right_shoot_vel, left_shoot_vel;

	p2Animation right_idle;
	p2Animation left_idle;
	p2Animation right_run;
	p2Animation left_run;
	p2Animation right_jump;
	p2Animation left_jump;
	p2Animation right_death_blink;
	p2Animation left_death_blink;
	p2Animation right_shoot;
	p2Animation left_shoot;

	//playerSoundEffects
	uint player_jump = 0;
	uint player_dead = 0;

	uint deadTime, shoot_timer;

	

	p2SString spritesheetName;
	bool reachedEnd;
	bool isDead;
	bool god_mode;
};

#endif