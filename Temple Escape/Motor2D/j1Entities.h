#ifndef __j1Entities_H__
#define __j1Entities_H__

#include "j1Module.h"
#include "p2Animation.h"

#define MAX_ENTITIES 500

enum ENTITY_TYPES
{
	BAT,
	SLIME,
	PLAYER,
	NO_TYPE
};

class Entity;
class Player;

struct EntityInfo
{
	ENTITY_TYPES type = ENTITY_TYPES::NO_TYPE;
	int x = 0, y = 0;
};

class j1Entities : public j1Module
{
public:

	j1Entities();
	~j1Entities();

	bool Awake(pugi::xml_node& conf);
	bool Start();
	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	bool Load(pugi::xml_node&);

	bool Save(pugi::xml_node&) const;

	// Called before quitting
	bool CleanUp();;

	void OnCollision(Collider* c1, Collider* c2);

	bool AddEntity(ENTITY_TYPES type, int x, int y);

	Player* GetPlayer()const;

private:

	void SpawnEntity(const EntityInfo& info);

private:

	EntityInfo queue[MAX_ENTITIES];

	Entity* entities[MAX_ENTITIES];

	SDL_Texture* entity_sprites = nullptr;

	p2SString spritesheetName;

};

#endif // __j1Enemies_H__