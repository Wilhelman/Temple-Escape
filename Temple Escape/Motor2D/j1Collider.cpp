#include "j1App.h"
#include "j1Input.h"
#include "j1Collider.h"
#include "p2Log.h"

j1Collider::j1Collider()
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		colliders[i] = nullptr;

	matrix[COLLIDER_GROUND][COLLIDER_GROUND] = false;
	matrix[COLLIDER_GROUND][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_GROUND][COLLIDER_DEAD] = false;
	matrix[COLLIDER_GROUND][COLLIDER_LVL_END] = false;
	matrix[COLLIDER_GROUND][COLLIDER_ENEMY_BAT] = false;
	matrix[COLLIDER_GROUND][COLLIDER_ENEMY_SLIME] = true;
	matrix[COLLIDER_GROUND][COLLIDER_PLAYER_BASIC_SHOT] = true;

	matrix[COLLIDER_PLAYER][COLLIDER_DEAD] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_GROUND] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_LVL_END] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_ENEMY_BAT] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_ENEMY_SLIME] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER_BASIC_SHOT] = false;

	matrix[COLLIDER_DEAD][COLLIDER_GROUND] = false;
	matrix[COLLIDER_DEAD][COLLIDER_DEAD] = false;
	matrix[COLLIDER_DEAD][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_DEAD][COLLIDER_LVL_END] = false;
	matrix[COLLIDER_DEAD][COLLIDER_ENEMY_BAT] = false;
	matrix[COLLIDER_DEAD][COLLIDER_ENEMY_SLIME] = false;
	matrix[COLLIDER_DEAD][COLLIDER_PLAYER_BASIC_SHOT] = false;

	matrix[COLLIDER_LVL_END][COLLIDER_LVL_END] = false;
	matrix[COLLIDER_LVL_END][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_LVL_END][COLLIDER_DEAD] = false;
	matrix[COLLIDER_LVL_END][COLLIDER_ENEMY_BAT] = false;
	matrix[COLLIDER_LVL_END][COLLIDER_ENEMY_SLIME] = false;
	matrix[COLLIDER_LVL_END][COLLIDER_PLAYER_BASIC_SHOT] = false;
	matrix[COLLIDER_LVL_END][COLLIDER_GROUND] = false;

	matrix[COLLIDER_ENEMY_BAT][COLLIDER_ENEMY_BAT] = false;
	matrix[COLLIDER_ENEMY_BAT][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_ENEMY_BAT][COLLIDER_DEAD] = false;
	matrix[COLLIDER_ENEMY_BAT][COLLIDER_LVL_END] = false;
	matrix[COLLIDER_ENEMY_BAT][COLLIDER_GROUND] = false;
	matrix[COLLIDER_ENEMY_BAT][COLLIDER_PLAYER_BASIC_SHOT] = true;
	matrix[COLLIDER_ENEMY_BAT][COLLIDER_ENEMY_SLIME] = false;

	matrix[COLLIDER_ENEMY_SLIME][COLLIDER_ENEMY_SLIME] = false;
	matrix[COLLIDER_ENEMY_SLIME][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_ENEMY_SLIME][COLLIDER_DEAD] = false;
	matrix[COLLIDER_ENEMY_SLIME][COLLIDER_LVL_END] = false;
	matrix[COLLIDER_ENEMY_SLIME][COLLIDER_GROUND] = true;
	matrix[COLLIDER_ENEMY_SLIME][COLLIDER_PLAYER_BASIC_SHOT] = true;
	matrix[COLLIDER_ENEMY_SLIME][COLLIDER_ENEMY_BAT] = false;

	matrix[COLLIDER_PLAYER_BASIC_SHOT][COLLIDER_ENEMY_BAT] = true;
	matrix[COLLIDER_PLAYER_BASIC_SHOT][COLLIDER_ENEMY_SLIME] = true;
	matrix[COLLIDER_PLAYER_BASIC_SHOT][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER_BASIC_SHOT][COLLIDER_DEAD] = false;
	matrix[COLLIDER_PLAYER_BASIC_SHOT][COLLIDER_LVL_END] = false;
	matrix[COLLIDER_PLAYER_BASIC_SHOT][COLLIDER_GROUND] = true;
	matrix[COLLIDER_PLAYER_BASIC_SHOT][COLLIDER_PLAYER_BASIC_SHOT] = false;

	name.create("collider");
}

// Destructor
j1Collider::~j1Collider()
{}

bool j1Collider::Awake() {
	
	return true;
}

bool j1Collider::Start() {
	return true;
}

bool j1Collider::PreUpdate()
{
	// Remove all colliders scheduled for deletion
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr && colliders[i]->to_delete == true)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}

// Called before render is available
bool j1Collider::Update(float dt)
{
	Collider* c1;
	Collider* c2;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		// skip empty colliders
		if (colliders[i] == nullptr)
			continue;

		c1 = colliders[i];

		// avoid checking collisions already checked
		for (uint k = i + 1; k < MAX_COLLIDERS; ++k)
		{
			// skip empty colliders
			if (colliders[k] == nullptr)
				continue;

			c2 = colliders[k];

			if (c1->CheckCollision(c2->rect) == true)
			{
				if (matrix[c1->type][c2->type] && c1->callback)
					c1->callback->OnCollision(c1, c2);

				if (matrix[c2->type][c1->type] && c2->callback)
					c2->callback->OnCollision(c2, c1);
			}
		}
	}

	DebugDraw();

	return true;
}

void j1Collider::DebugDraw()
{
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		debug = !debug;

	if (debug == false)
		return;

	Uint8 alpha = 80;
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
			continue;

		switch (colliders[i]->type)
		{
		case COLLIDER_NONE: // white
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 255, alpha);
			break;
		case COLLIDER_GROUND: // blue
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 255, alpha);
			break;
		case COLLIDER_DEAD: // red
			App->render->DrawQuad(colliders[i]->rect, 255, 0, 0, alpha);
			break;
		case COLLIDER_LVL_END: // unknow
			App->render->DrawQuad(colliders[i]->rect, 100, 100, 0, alpha);
			break;
		case COLLIDER_PLAYER: // green
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 0, alpha);
			break;
		case COLLIDER_ENEMY_BAT: // green
			App->render->DrawQuad(colliders[i]->rect, 50, 255, 50, alpha);
			break;
		case COLLIDER_ENEMY_SLIME: // green
			App->render->DrawQuad(colliders[i]->rect, 50, 255, 50, alpha);
			break;
		case COLLIDER_PLAYER_BASIC_SHOT: // red
			App->render->DrawQuad(colliders[i]->rect, 255, 0, 0, alpha);
			break;
		default:
			break;
		}
	}
}

// Called before quitting
bool j1Collider::CleanUp()
{
	LOG("Freeing all colliders");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}

Collider* j1Collider::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback)
{
	Collider* ret = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rect, type, callback);
			break;
		}
	}

	return ret;
}

bool j1Collider::EraseCollider(Collider* collider)
{
	if (collider != nullptr)
	{
		for (uint i = 0; i < MAX_COLLIDERS; ++i)
		{
			if (colliders[i] == collider)
			{
				collider->to_delete = true;
				break;
			}
		}
	}


	return false;
}

// -----------------------------------------------------

bool Collider::CheckCollision(const SDL_Rect& r) const
{
	return (rect.x < r.x + r.w &&
			rect.x + rect.w > r.x &&
			rect.y < r.y + r.h &&
			rect.h + rect.y > r.y);
}

