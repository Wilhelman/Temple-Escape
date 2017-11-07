#ifndef __Bat_H__
#define __Bat_H__

#include "Enemy.h"
#include "p2Animation.h"

class Bat : public Enemy
{
private:
	p2Animation fly_r,fly_l;
	fPoint original_pos;
	int bat_IA; //lol
	bool bat_going_right, moving;
	fPoint movementSpeed = { 0.5f, 0.0f };
	iPoint movementGoal;

public:

	Bat(int x, int y);
	void OnCollision(Collider* collider);
	void Move();
};

#endif