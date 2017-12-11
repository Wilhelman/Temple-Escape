#include "j1App.h"
#include "Coin.h"
#include "j1Collider.h"
#include "j1Map.h"
#include "Player.h"
#include "p2Log.h"
#include "j1Pathfinding.h"
#include "j1Entities.h"
#include "j1Audio.h"

Coin::Coin(int x, int y) : Entity(x, y)
{
	bool ret = true;

	//TODO : this looks ugly but it seems the only way :'c
	pugi::xml_document	config_file;
	pugi::xml_node* node = &App->LoadConfig(config_file); //todo: make a method to get the root without passing the xml_document
	node = &node->child("entities").child("coin");

	//read fxs from node
	coin_collect_fx = App->audio->LoadFx(node->child("fxCoinCollect").attribute("name").as_string());

	//read animation from node
	for (pugi::xml_node animations = node->child("animations").child("animation"); animations && ret; animations = animations.next_sibling("animation"))
	{
		p2SString tmp(animations.attribute("name").as_string());

		if (tmp == "idle")
			LoadAnimation(animations, &idle);

	}

	animation = &idle;

	original_pos = App->map->WorldToMap(x, y);

	collider = App->collider->AddCollider({ 0, 0, 16, 16 }, ColliderType::COLLIDER_COIN, (j1Module*)App->entities);

}

Coin::~Coin()
{
	LOG("Freeing the coin");

	LOG("Unloading coin sound fx");
	App->audio->UnLoadFx(coin_collect_fx);

}

void Coin::LoadAnimation(pugi::xml_node animation_node, p2Animation* animation) {
	bool ret = true;

	for (pugi::xml_node frame = animation_node.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
		animation->PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

	animation->speed = animation_node.attribute("speed").as_float();
	animation->loop = animation_node.attribute("loop").as_bool();
}

void Coin::Update(float dt)
{

	if (!key_entities_speed && dt > 0)
		SetEntitiesSpeed(dt);

	if (have_to_destroy && current_time > dead_timer + 100)
		this->to_destroy = true;

	current_time = SDL_GetTicks();
}

void Coin::OnCollision(Collider* collider)
{
	if (collider->type == ColliderType::COLLIDER_PLAYER)
	{
		App->audio->PlayFx(coin_collect_fx);
		have_to_destroy = true;
		dead_timer = SDL_GetTicks();
		//TODO GIVE SCORE ETC
	}
}

void Coin::SetEntitiesSpeed(float dt) {
	idle_vel = idle.speed;
	key_entities_speed = true;
}

bool Coin::Save(pugi::xml_node& save) const
{
	bool ret = true;

	if (save.child("position").empty())
	{
		save = save.append_child("position");
		save.append_attribute("x").set_value(position.x);
		save.append_attribute("y").set_value(position.y);
	}
	else
	{
		save.child("position").attribute("x").set_value(position.x);
		save.child("position").attribute("y").set_value(position.y);
	}

	return ret;
}