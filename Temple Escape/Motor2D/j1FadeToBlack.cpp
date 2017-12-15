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
#include "j1Pathfinding.h"

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
	if (current_step == FadeStep::NONE_FADE)
		return ret;

	Uint32 now = SDL_GetTicks() - start_time;
	float normalized = MIN(1.0f, (float)now / (float)total_time);

	switch (current_step)
	{
	case FadeStep::FADE_TO_BLACK:
	{
		if (now >= total_time)
		{

			if (moduleOn != nullptr && moduleOff != nullptr) {
				this->moduleOff->CleanUp();
				this->moduleOff->active = false;
				
				this->moduleOn->Start();
				this->moduleOn->active = true;
				
			}

			App->entities->CleanUp();
			App->entities->active = false;
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
				if (moduleOn != (j1Module*)App->main_menu) {
					for (lvlData = lvlData.child("map").child("scene").child("data"); lvlData && ret; lvlData = lvlData.next_sibling("data"))
					{
						if (lvlName == "")
						{
							if (lvlData.previous_sibling("data") == NULL)
								firstData = lvlData.next_sibling("data");

							if (loadTheNextOne)
							{
								lvlData.attribute("currentLvl").set_value(true);
								nextLvlName = (p2SString)lvlData.attribute("name").as_string();
								loaded = true;
								break;
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

				}
				else {
					for (lvlData = lvlData.child("map").child("scene").child("data"); lvlData && ret; lvlData = lvlData.next_sibling("data"))
					{
						if (lvlData.attribute("currentLvl").as_bool())
						{
							lvlData.attribute("currentLvl").set_value(false);
						}

						p2SString tmp = (p2SString)lvlData.attribute("name").as_string();
						p2SString main_menu;
						lvlName = main_menu.create("main_menu.tmx");

						if (tmp == main_menu)
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
				if (moduleOn == (j1Module*)App->main_menu) {
					App->map->spawn.x = -1;
					App->map->spawn.y = -1;
				}

				int w, h;
				uchar* data_map = NULL;
				if (App->map->CreateWalkabilityMap(w, h, &data_map))
					App->pathfinding->SetMap(w, h, data_map);

				RELEASE_ARRAY(data_map);

			
			App->entities->Start();
			App->entities->active = true;
			if (have_to_load)
				App->LoadGame();
			App->particles->Start();

			

			if ((lvlName != "" && !F1 && (moduleOn == nullptr && moduleOff == nullptr)) || have_to_load)
				App->entities->GetPlayer()->ImplementLoad();

			F1 = false;

			moduleOn = nullptr;
			moduleOff = nullptr;
			lvlName = "";
			have_to_load = false;

			total_time += total_time;
			start_time = SDL_GetTicks();
			current_step = FadeStep::FADE_FROM_BLACK;

		}
	} break;

	case FadeStep::FADE_FROM_BLACK:
	{
		normalized = 1.0f - normalized;

		if (now >= total_time)
			current_step = FadeStep::NONE_FADE;
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
	if (current_step == FadeStep::NONE_FADE)
	{
		current_step = FadeStep::FADE_TO_BLACK;
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
	if (current_step == FadeStep::NONE_FADE)
	{
		current_step = FadeStep::FADE_TO_BLACK;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);
		ret = true;
	}

	return ret;
}

bool j1FadeToBlack::FadeToBlackBetweenModules(j1Module* module_off, j1Module* module_on, float time, bool andLoad)
{
	bool ret = false;

	have_to_load = andLoad;
	moduleOff = module_off;
	moduleOn = module_on;

	if (current_step == FadeStep::NONE_FADE)
	{
		current_step = FadeStep::FADE_TO_BLACK;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);
		ret = true;
	}

	return ret;
}

bool j1FadeToBlack::FadeIsOver() {
	bool ret = true;

	if (current_step == FadeStep::NONE_FADE)
		ret = true;
	else
		ret = false;

	return ret;
}