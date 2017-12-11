#ifndef __Slime_H__
#define __Slime_H__

#include "Entity.h"
#include "p2Animation.h"
#include "p2DynArray.h"

#define TILE_RADAR 40

class Slime : public Entity
{
private:
	bool slime_going_right = true, moving = false, player_in_radar = false;
	fPoint movementSpeed = fPoint(0.0f,0.0f);
	iPoint movementGoal = iPoint(0, 0);
	iPoint playerGoal = iPoint(0, 0);

	iPoint tile_radar[TILE_RADAR];

	//timers
	uint current_time = 0u, slime_time_chilling = 0u;

	// dead control
	int dead_timer = 0u;
	bool have_to_destroy = false;

	//anims
	p2Animation standard_left_idle = p2Animation();
	p2Animation standard_right_idle = p2Animation();
	p2Animation standard_left_jump = p2Animation();
	p2Animation standard_right_jump = p2Animation();

	//anims speeds
	uint standard_left_idle_vel = 0u, standard_right_idle_vel = 0u, standard_right_jump_vel = 0u, standard_left_jump_vel = 0u;

	//slime_fx
	uint slime_hit_fx = 0u, slime_dead_fx = 0u;

public:

	Slime(int x, int y);
	~Slime();
	void OnCollision(Collider* collider);
	void Update(float dt);
	void SetEntitiesSpeed(float dt);
	void LoadAnimation(pugi::xml_node animation_node, p2Animation* animation);
	void SetRadar();
	bool CheckForPlayer();
	uint getLives();

	bool Save(pugi::xml_node&) const;

	void SetMovementWithPath(const p2DynArray<iPoint>* path, float dt, iPoint position);
};

#endif