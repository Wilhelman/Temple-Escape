#include <math.h>
#include "j1App.h"
#include "j1FadeToBlack.h"
#include "j1Render.h"
#include "SDL/include/SDL_render.h"
#include "SDL/include/SDL_timer.h"
#include "j1Window.h"
#include "j1Collider.h"
#include "j1Map.h"
#include "Player.h"
#include "j1Entities.h"
#include "j1Particles.h"
#include "p2Log.h"

j1FadeToBlack::j1FadeToBlack()
{
	name.create("fadeToBlack");
}

j1FadeToBlack::~j1FadeToBlack()
{}

bool j1FadeToBlack::Awake(pugi::xml_node& conf) 
{
	uint winWidth, winHeight;

	App->win->GetWindowSize(winWidth, winHeight);

	screen = { 0, 0,  (int)winWidth * (int)App->win->GetScale(), (int)winHeight * (int)App->win->GetScale() };

	return true;
}

// Load assets
bool j1FadeToBlack::Start()
{
	LOG("Preparing Fade Screen");
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	lvlName = "";
	F1 = false;
	return true;
}

// Update: draw background
bool j1FadeToBlack::Update(float dt)
{
	bool ret = true;
	if (current_step == fade_step::none)
		return ret;

	Uint32 now = SDL_GetTicks() - start_time;
	float normalized = MIN(1.0f, (float)now / (float)total_time);

	switch (current_step)
	{
	case fade_step::fade_to_black:
	{
		if (now >= total_time)
		{
			App->entities->CleanUp();
			//App->player->CleanUp();
			App->particles->CleanUp();
			App->collider->CleanUp();
			App->map->CleanUp();
			pugi::xml_node firstData;
			pugi::xml_document data;
			
			pugi::xml_node lvlData = App->LoadConfig(data);
			p2SString nextLvlName = "";

			bool loadTheNextOne = false;
			bool setFirst = false;
			bool loaded = false;

			for (lvlData = lvlData.child("map").child("scene").child("data"); lvlData && ret; lvlData = lvlData.next_sibling("data"))
			{
				if (lvlName == "") 
				{
					if (lvlData.previous_sibling("data") == NULL)
						firstData = lvlData;

					if (loadTheNextOne) 
					{
						lvlData.attribute("currentLvl").set_value(true);
						nextLvlName = (p2SString)lvlData.attribute("name").as_string();
						loaded = true;
					}

					if (lvlData.attribute("currentLvl").as_bool() && !loadTheNextOne) 
					{
						lvlData.attribute("currentLvl").set_value(false);
						loadTheNextOne = true;
					}

					if (lvlData.next_sibling("data") == NULL && !loaded) {
						setFirst = true;
					}
				}
				else 
				{
					if (lvlData.attribute("currentLvl").as_bool()) 
					{
						lvlData.attribute("currentLvl").set_value(false);
					}

					p2SString tmp = (p2SString)lvlData.attribute("name").as_string();

					if (tmp == lvlName) 
					{
						lvlData.attribute("currentLvl").set_value(true);
					}
				}
			}
			if (setFirst) 
			{
				firstData.attribute("currentLvl").set_value(true);
				nextLvlName = (p2SString)firstData.attribute("name").as_string();
			}

			//We save the xml to do a loop through all the lvl's without knowing the number
			data.save_file("config.xml");
			
			if (lvlName != "")
				nextLvlName = lvlName;

			App->map->sceneName = nextLvlName;
			App->map->Load(nextLvlName.GetString());
			App->map->LayersSetUp();
			
			App->map->setAllLogicForMap();
			//App->player->Start();
			App->entities->Start();
			App->particles->Start();

			if(lvlName != "" && !F1)
				App->entities->player->ImplementLoad();

			F1 = false;
			
			total_time += total_time;
			start_time = SDL_GetTicks();
			current_step = fade_step::fade_from_black;
		}
	} break;

	case fade_step::fade_from_black:
	{
		normalized = 1.0f - normalized;

		if (now >= total_time)
			current_step = fade_step::none;
	} break;
	}

	// Finally render the black square with alpha on the screen
	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0f));
	SDL_RenderFillRect(App->render->renderer, &screen);

	return ret;
}

// Fade to black. At mid point deactivate one module, then activate the other
bool j1FadeToBlack::FadeToBlack(float time)
{
	lvlName = "";
	bool ret = false;
	if (current_step == fade_step::none)
	{
		current_step = fade_step::fade_to_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);
		ret = true;
	}

	return ret;
}

bool j1FadeToBlack::FadeToKnowBlack(p2SString lvlToFade, float time)
{
	lvlName = lvlToFade;
	bool ret = false;
	if (current_step == fade_step::none)
	{
		current_step = fade_step::fade_to_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);
		ret = true;
	}

	return ret;
}

bool j1FadeToBlack::FadeIsOver() {
	bool ret = true;

	if (current_step == fade_step::none)
		ret = true;
	else
		ret = false;

	return ret;
}