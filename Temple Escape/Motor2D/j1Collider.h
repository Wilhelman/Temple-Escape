#ifndef __j1Collider_H__
#define __j1Collider_H__

#define MAX_COLLIDERS 1000

#include "j1Module.h"

#include "j1Render.h"


enum ColliderType
{
	COLLIDER_NONE = -1,
	COLLIDER_GROUND,
	COLLIDER_DEAD,
	COLLIDER_LVL_END,
	COLLIDER_PLAYER,
	COLLIDER_ENEMY_BAT,
	COLLIDER_ENEMY_SLIME,
	COLLIDER_PLAYER_BASIC_SHOT,

	COLLIDER_MAX
};

struct Collider
{
	SDL_Rect rect = {0, 0, 0, 0};
	bool to_delete = false;
	ColliderType type = COLLIDER_NONE;
	j1Module* callback = nullptr;

	Collider(SDL_Rect rectangle, ColliderType type, j1Module* callback = nullptr) :
		rect(rectangle),
		type(type),
		callback(callback)
	{}

	void SetPos(int x, int y)
	{
		rect.x = x;
		rect.y = y;
	}

	bool CheckCollision(const SDL_Rect& r) const;
};

class j1Collider : public j1Module
{
public:

	j1Collider();
	~j1Collider();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();


	Collider* AddCollider(SDL_Rect rect, ColliderType type, j1Module* callback = nullptr);
	void DebugDraw();

	bool EraseCollider(Collider* collider);

private:

	Collider* colliders[MAX_COLLIDERS];
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
	bool debug = false;
};

#endif // __ModuleCollision_H__