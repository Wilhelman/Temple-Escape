#ifndef __j1Enemies_H__
#define __j1Enemies_H__

#include "j1Module.h"
#include "p2Animation.h"

#define MAX_ENEMIES 500

enum ENEMY_TYPES
{
	NO_TYPE,
	BAT,
	SLIME
};

class Enemy;

struct EnemyInfo
{
	ENEMY_TYPES type = ENEMY_TYPES::NO_TYPE;
	int x, y;
};

class j1Enemies : public j1Module
{
public:

	j1Enemies();
	~j1Enemies();

	bool Awake(pugi::xml_node& conf);
	bool Start();
	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();;
	void OnCollision(Collider* c1, Collider* c2);

	bool AddEnemy(ENEMY_TYPES type, int x, int y);

	p2Animation slime_right_jump;
	p2Animation slime_left_jump;

private:

	void SpawnEnemy(const EnemyInfo& info);

private:
	EnemyInfo queue[MAX_ENEMIES];
	Enemy* enemies[MAX_ENEMIES];
	SDL_Texture* enemy_sprites;

	uint screen_scale;
	uint screen_width;
	uint screen_height;

	p2SString spritesheetName;

};

#endif // __j1Enemies_H__