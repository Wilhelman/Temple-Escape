#ifndef __Slime_H__
#define __Slime_H__

#include "Enemy.h"
#include "p2Animation.h"
#include "p2DynArray.h"

#define TILE_RADAR 40
#define GRAVITY 40
#define SLIME_SPEED 20

class Slime : public Enemy
{
private:
	fPoint original_pos;
	int slime_IA;
	bool slime_going_right, moving, player_in_radar, know_go;
	fPoint movementSpeed;
	iPoint movementGoal, playerGoal;

	iPoint tile_radar[TILE_RADAR];

	uint current_time, slime_time_chilling;

public:

	Slime(int x, int y);
	void OnCollision(Collider* collider);
	void Move(float dt);
	void SetEntitiesSpeed(float dt);
	void SetRadar();
	bool CheckForPlayer();
	uint getLives();

	void SetMovementWithPath(const p2DynArray<iPoint>* path, float dt, iPoint position);

	//animations
	p2Animation standard_left_idle;
	p2Animation standard_right_idle;
	p2Animation standard_left_jump;
	p2Animation standard_right_jump;
};

#endif