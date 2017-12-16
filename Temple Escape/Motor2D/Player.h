#ifndef __j1Player_H__
#define __j1Player_H__

#include "p2Animation.h"
#include "p2Point.h"
#include "Entity.h"

#define PLAYER_1_LIVE { 14, 335, 42, 14 }
#define PLAYER_2_LIVE { 14, 349, 42, 14 }
#define PLAYER_3_LIVE { 14, 363, 42, 14 }
#define PLAYER_4_LIVE { 14, 377, 42, 14 }
#define PLAYER_5_LIVE { 14, 391, 42, 14 }
#define PLAYER_6_LIVE { 14, 405, 42, 14 }
#define PLAYER_HEART { 4, 337, 10, 10 }


class j1Timer;
struct SDL_Texture;
struct Collider;

static fPoint position_implement_load = fPoint();
static int lives_implement_load = 0;
static int score_implement_load = 0;
static int time_implement_load = 0;

class Player : public Entity
{
	enum PlayerState
	{
		ST_IDLE_RIGHT,
		ST_IDLE_LEFT,
		ST_RUN_RIGHT,
		ST_RUN_LEFT,
		ST_JUMP_RIGHT,
		ST_JUMP_LEFT,
		ST_SHOOT_RIGHT,
		ST_SHOOT_LEFT,

		ST_UNKNOWN
	};

	enum PlayerLastState
	{
		LAST_ST_IDLE_RIGHT,
		LAST_ST_IDLE_LEFT,
		LAST_ST_RUN_RIGHT,
		LAST_ST_RUN_LEFT,
		LAST_ST_JUMP_RIGHT,
		LAST_ST_JUMP_LEFT,
		LAST_ST_SHOOT_RIGHT,
		LAST_ST_SHOOT_LEFT,

		LAST_ST_UNKNOWN
	};

	enum Direction 
	{
		RIGHT,
		LEFT,
		UP,
		DOWN,

		NO_DIR
	};

public:

	SDL_Rect current_frame = { 0,0,0,0 };

	bool reachedEnd = false;
	bool isDead = false;
	bool god_mode = false;
	bool god_mode_fly = false;
	int p_lives = 3;
	int score = 0;
	int timer = 0;

private:
	j1Timer real_timer;

	bool didDoubleJump = false;;
	bool isJumping = false;
	bool player_appeared = false;

	//timers
	uint currentTime = 0u;
	uint jumpTimer = 0u;
	uint deadTime = 0u;
	uint shoot_timer = 0u;

	float current_dt = 0.0f;

	iPoint last_save_position = iPoint(0,0);

	PlayerState current_state = PlayerState::ST_IDLE_RIGHT;
	PlayerLastState last_state = PlayerLastState::LAST_ST_RUN_RIGHT;

	//playerSoundEffects
	uint player_jump_fx = 0u;
	uint player_dead_fx = 0u;
	uint player_life_reward_fx = 0u;

	int last_score = 0;

	//anims
	p2Animation right_idle = p2Animation();
	p2Animation left_idle = p2Animation();
	p2Animation right_run = p2Animation();
	p2Animation left_run = p2Animation();
	p2Animation right_jump = p2Animation();
	p2Animation left_jump = p2Animation();
	p2Animation right_death_blink = p2Animation();
	p2Animation left_death_blink = p2Animation();
	p2Animation right_shoot = p2Animation();
	p2Animation left_shoot = p2Animation();

	//anims speed
	uint right_idle_vel = 0u, left_idle_vel = 0u, right_run_vel = 0u, left_run_vel = 0u, right_jump_vel = 0u,
		left_jump_vel = 0u, right_death_blink_vel = 0u, left_death_blink_vel = 0u, right_shoot_vel = 0u, left_shoot_vel = 0u;

private:

	float gravityHaveToWork();
	float canGoRight();
	float canGoLeft();
	float canGoUp();
	bool PlayerCanShootRight();
	bool PlayerCanShootLeft();
	float DistanceToWall(SDL_Rect wall, SDL_Rect player, Direction direction);

	void SetPlayerAnimation(PlayerState current_state, PlayerLastState last_state);
	void SetPlayerStates(PlayerState current_state, PlayerLastState last_state);
	void SetPlayerAnimationsSpeed(float dt);

public:

	Player(int x, int y);
	~Player();

	void Update(float dt);

	void SetEntitiesSpeed(float dt);

	void LoadAnimation(pugi::xml_node animation_node, p2Animation* animation);

	void OnCollision(Collider* collider);

	bool Load(pugi::xml_node&);

	void ImplementLoad();

	bool Save(pugi::xml_node&) const;

};

#endif