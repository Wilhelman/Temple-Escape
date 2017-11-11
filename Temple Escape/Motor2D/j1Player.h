#ifndef __j1Player_H__
#define __j1Player_H__

#include "j1Module.h"
#include "p2Animation.h"
#include "p2Point.h"

struct SDL_Texture;
struct Collider;


class j1Player : public j1Module
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

private:
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

	Collider* pCollider;
	bool didDoubleJump;
	bool isJumping;
	uint deadTime;
	uint currentTime;
	uint jumpTimer;

	//playerSoundEffects
	uint player_jump = 0;
	uint player_dead = 0;

private:
	float gravityHaveToWork();
	bool canGoRight();
	bool canGoLeft();
	bool canGoUp();
	bool CheckCollision(SDL_Rect A, SDL_Rect B);

public:
	j1Player();
	~j1Player();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void OnCollision(Collider* c1, Collider* c2);

	bool Load(pugi::xml_node&);

	void ImplementLoad();

	bool Save(pugi::xml_node&) const;

public:
	SDL_Texture* graphics = nullptr;
	PlayerState current_state;
	PlayerLastState last_state;
	p2Animation* current_animation;
	fPoint position;
	fPoint tmp;

	p2SString spritesheetName;
	p2SString fxPlayerDead;
	p2SString fxPlayerJump;

	bool reachedEnd;
	bool isDead;
};

#endif