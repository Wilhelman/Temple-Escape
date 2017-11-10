#ifndef __Bat_H__
#define __Bat_H__

#include "Enemy.h"
#include "p2Animation.h"

#define TILE_RADAR 56
#define BAT_LIVES 2

class Bat : public Enemy
{
private:
	iPoint original_pos;
	int bat_IA; //lol
	bool bat_going_right, moving, player_in_radar, have_to_chill;
	fPoint movementSpeed;
	iPoint movementGoal, playerGoal;

	iPoint tile_radar[TILE_RADAR];

	bool dead;

public:

	Bat(int x, int y);
	void OnCollision(Collider* collider);
	void Move();
	void SetRadar();
	bool CheckForPlayer();
	uint getLives();
	void resetLives();


	
};

#endif