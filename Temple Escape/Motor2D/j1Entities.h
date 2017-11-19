#ifndef __j1Entities_H__
#define __j1Entities_H__

#include "j1Module.h"
#include "p2Animation.h"

#define MAX_ENTITIES 500

enum ENTITY_TYPES
{
	NO_TYPE,
	BAT,
	SLIME,
	PLAYER
};

class Entity;
class Player;

struct EntityInfo
{
	ENTITY_TYPES type = ENTITY_TYPES::NO_TYPE;
	int x, y;
};

class j1Entities : public j1Module
{
public:

	j1Entities();
	~j1Entities();

	bool Awake(pugi::xml_node& conf);
	bool Start();
	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();;
	void OnCollision(Collider* c1, Collider* c2);

	bool AddEntity(ENTITY_TYPES type, int x, int y);

	//PLAYER
	p2Animation player_right_idle;
	p2Animation player_left_idle;
	p2Animation player_right_jump;
	p2Animation player_left_jump;
	p2Animation player_right_run;
	p2Animation player_left_run;
	p2Animation player_right_dead;
	p2Animation player_left_dead;
	p2Animation player_right_shoot;
	p2Animation player_left_shoot;

	//SLIME
	p2Animation slime_right_idle;
	p2Animation slime_left_idle;
	p2Animation slime_right_jump;
	p2Animation slime_left_jump;

	//BAT
	p2Animation bat_fly_right;
	p2Animation bat_fly_left;

	Player* player;

private:

	void SpawnEntity(const EntityInfo& info);

private:
	EntityInfo queue[MAX_ENTITIES];
	Entity* entities[MAX_ENTITIES];
	SDL_Texture* entity_sprites;

	uint screen_scale;
	uint screen_width;
	uint screen_height;

	uint player_jump, player_dead;

	p2SString spritesheetName;
	p2SString fxPlayerJump;
	p2SString fxPlayerDead;

};

#endif // __j1Enemies_H__