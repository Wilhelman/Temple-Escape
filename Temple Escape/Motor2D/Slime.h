#ifndef __Slime_H__
#define __Slime_H__

#include "Enemy.h"
#include "p2Animation.h"

class Slime : public Enemy
{
private:
	fPoint original_pos;
	int slime_IA; 
	bool slime_going_right, moving, dead;
	fPoint movementSpeed = { 0.5f, 0.0f };
	iPoint movementGoal;

public:

	Slime(int x, int y);
	void OnCollision(Collider* collider);
	void Move(float dt);
	uint getLives();
};

#endif