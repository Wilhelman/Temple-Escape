#ifndef __j1Entities_H__
#define __j1Entities_H__

#include "j1Module.h"
#include "p2Animation.h"
#include "p2DynArray.h"

enum EntityType
{
	BAT,
	SLIME,
	PLAYER,
	COIN,

	NO_TYPE
};

class Entity;
class Player;

class j1Entities : public j1Module
{
public:

	j1Entities();
	~j1Entities();

	bool Awake(pugi::xml_node& conf);

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool Load(pugi::xml_node&);

	bool Save(pugi::xml_node&) const;

	bool CleanUp();

	void OnCollision(Collider* c1, Collider* c2);

	Player* GetPlayer()const;

	bool SpawnEntity(int x, int y, EntityType type);

private:

	p2DynArray<Entity*> entities;

	SDL_Texture* entity_sprites = nullptr;

	p2SString spritesheetName;

};

#endif // __j1Enemies_H__