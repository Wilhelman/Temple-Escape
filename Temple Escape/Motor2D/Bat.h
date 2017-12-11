#ifndef __Bat_H__
#define __Bat_H__

#include "Entity.h"
#include "p2Animation.h"
#include "p2DynArray.h"

#define TILE_RADAR 48

class Bat : public Entity
{
private:
	iPoint original_pos = iPoint(0, 0);
	int bat_IA = 1;
	bool bat_going_right = true;
	bool moving = false, player_in_radar = false, have_to_chill = false;

	fPoint movementSpeed = fPoint(0.0f, 0.0f);
	iPoint movementGoal = iPoint(0, 0);
	iPoint playerGoal = iPoint(0, 0);

	iPoint tile_radar[TILE_RADAR];

	// dead control
	uint dead_timer = 0u;
	bool have_to_destroy = false;

	//bat_fx
	uint bat_hit_fx = 0u;
	uint bat_dead_fx = 0u;

	//animations
	p2Animation standard_right_fly = p2Animation();
	p2Animation standard_left_fly = p2Animation();

	//animations speeds
	uint standard_right_fly_vel = 0u, standard_left_fly_vel = 0u;

public:

	Bat(int x, int y);
	~Bat();

	void OnCollision(Collider* collider);
	void Update(float dt);
	void SetRadar();
	bool CheckForPlayer();
	uint getLives();

	bool Save(pugi::xml_node&) const;
	void LoadAnimation(pugi::xml_node animation_node, p2Animation* animation);
	void SetMovementWithPath(const p2DynArray<iPoint>* path, float dt, iPoint position);
	void SetEntitiesSpeed(float dt);

};

#endif