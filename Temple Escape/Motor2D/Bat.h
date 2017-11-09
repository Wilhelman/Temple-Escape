#ifndef __Bat_H__
#define __Bat_H__

#include "Enemy.h"
#include "p2Animation.h"

#define TILE_RADAR 56

class Bat : public Enemy
{
private:
	p2Animation fly_r,fly_l;
	iPoint original_pos;
	int bat_IA; //lol
	bool bat_going_right, moving, player_in_radar, have_to_chill;
	fPoint movementSpeed;
	iPoint movementGoal, playerGoal;

	iPoint tile_radar[TILE_RADAR];

public:

	Bat(int x, int y);
	void OnCollision(Collider* collider);
	void Move();
	void SetRadar();
	bool CheckForPlayer();
};

#endif