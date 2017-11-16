#ifndef __Bat_H__
#define __Bat_H__

#include "Enemy.h"
#include "p2Animation.h"
#include "p2DynArray.h"

#define TILE_RADAR 48

class Bat : public Enemy
{
private:
	iPoint original_pos;
	int bat_IA;
	bool bat_going_right, moving, player_in_radar, have_to_chill;
	fPoint movementSpeed;
	iPoint movementGoal, playerGoal;

	iPoint tile_radar[TILE_RADAR];

public:

	Bat(int x, int y);
	void OnCollision(Collider* collider);
	void Move(float dt);
	void SetRadar();
	bool CheckForPlayer();
	uint getLives();

	void SetMovementWithPath(const p2DynArray<iPoint>* path, float dt, iPoint position);
	void SetEntitiesSpeed(float dt);

	//animations
	p2Animation standard_right_fly;
	p2Animation standard_left_fly;

};

#endif