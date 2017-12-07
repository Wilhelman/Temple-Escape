#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1MainMenu.h"
#include "j1Scene.h"
#include "j1Collider.h"
#include "j1FadeToBlack.h"
#include "j1Pathfinding.h"
#include "Player.h"
#include "j1UI.h"

#include "UIImage.h"
#include "UIButton.h"

#include "j1Entities.h"

j1MainMenu::j1MainMenu() : j1Module()
{
	name.create("main_menu");
}

// Destructor
j1MainMenu::~j1MainMenu()
{}

// Called before render is available

bool j1MainMenu::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1MainMenu::Start()
{
	bool ret = true;

	UIImage* window = (UIImage*)App->ui->AddUIImage(100, 100, { 0, 512, 483, 512 }, this);
	window->draggable = true;
	buttons.PushBack((UIButton*)App->ui->AddUIButton(150, 150, { 0,113,229,69 }, { 411,169,229,69 }, { 642,169,229,69 }, this, window));
	UIButton* draggable_btn = (UIButton*)App->ui->AddUIButton(150, 190, { 0,113,229,69 }, { 411,169,229,69 }, { 642,169,229,69 }, this, window);
	draggable_btn->draggable = true;
	buttons.PushBack(draggable_btn);

	return ret;
}

// Called each loop iteration
bool j1MainMenu::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1MainMenu::Update(float dt)
{
	//App->win->SetTitle(title.GetString()); // TODO: remove this if not needed and above too

	/*if (App->entities->GetPlayer()->reachedEnd && App->fadeToBlack->FadeIsOver())
	{
		App->entities->GetPlayer()->reachedEnd = false;
		App->fadeToBlack->FadeToBlack();
	}*/

	return true;
}

// Called each loop iteration
bool j1MainMenu::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1MainMenu::CleanUp()
{
	LOG("Freeing main_menu");

	return true;
}

bool j1MainMenu::Load(pugi::xml_node& load)
{
	bool ret = true;

	if (!load.child("currentLvl").empty())
	{
		p2SString lvlToLoad = (p2SString)load.child("currentLvl").attribute("name").as_string();

		if (App->map->sceneName != lvlToLoad && App->fadeToBlack->FadeIsOver())
		{
			App->entities->GetPlayer()->reachedEnd = false;
			App->fadeToBlack->FadeToKnowBlack(lvlToLoad);
		}
	}

	return ret;
}

bool j1MainMenu::Save(pugi::xml_node& save) const
{
	bool ret = true;

	if (save.child("currentLvl").empty())
	{
		save = save.append_child("currentLvl");
		p2SString tmp = App->map->sceneName;
		save.append_attribute("name").set_value(tmp.GetString());
	}
	else
	{
		p2SString tmp = App->map->sceneName;
		save.child("currentLvl").attribute("name").set_value(tmp.GetString());
	}

	return ret;
}

void j1MainMenu::OnUITrigger(UIElement* elementTriggered, UI_State ui_state) {
	if (elementTriggered->type == IMAGE) {
		UIImage* tmpImg = (UIImage*)elementTriggered;
		switch (ui_state)
		{
		case STATE_NORMAL:
			break;
		case STATE_MOUSE_ENTER:
			break;
		case STATE_MOUSE_LEAVE:
			break;
		case STATE_LEFT_MOUSE_PRESSED:
			break;
		case STATE_NO_DEF:
			break;
		default:
			break;
		}
	}
	else if (elementTriggered->type == BUTTON) {
		UIButton* tmpBtn = (UIButton*)elementTriggered;
		switch (ui_state)
		{
		case STATE_NORMAL:
			break;
		case STATE_MOUSE_ENTER: {
			for (int i = 0; i < buttons.Count(); i++)
			{
				if (buttons[i]->current_state == STATE_FOCUSED) {
					buttons[i]->current_state = STATE_NORMAL;
					tmpBtn->UpdateButtonWithSelfRect(tmpBtn->btn_normal);
					break;
				}
			}
			tmpBtn->UpdateButtonWithSelfRect(tmpBtn->btn_focused);
			break;
		}
		case STATE_MOUSE_LEAVE:
			tmpBtn->UpdateButtonWithSelfRect(tmpBtn->btn_normal);
			break;
		case STATE_LEFT_MOUSE_PRESSED:
			tmpBtn->UpdateButtonWithSelfRect(tmpBtn->btn_pressed);
			break;
		case STATE_NO_DEF:
			break;
		default:
			break;
		}
	}
}