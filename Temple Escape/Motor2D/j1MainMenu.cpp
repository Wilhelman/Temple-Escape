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
#include "UILabel.h"

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

	uint win_width = 0u, win_height = 0u;
	App->win->GetWindowSize(win_width, win_height);
	win_width /= App->win->GetScale();
	win_height /= App->win->GetScale();

	// MAIN MENU BUTTONS
	new_game_btn = (UIButton*)App->ui->AddUIButton(win_width / 2 - 62, win_height / 2 - 25, { 0,32,123,32 }, { 320,105,135,44 }, { 0,64,124,29 }, this);
	buttons.PushBack(new_game_btn);

	UILabel* new_game_lbl = (UILabel*)App->ui->AddUILabel(10,10, "NEW GAME", BLACK, new_game_btn);
	new_game_btn->button_lbl = new_game_lbl;
	labels.PushBack(new_game_lbl);
	new_game_lbl->interactable = false;



	settings_btn = (UIButton*)App->ui->AddUIButton(win_width / 2 - 62, win_height / 2 - 16 + 42, { 0,32,123,32 }, { 320,105,135,44 }, { 0,64,124,29 }, this);
	buttons.PushBack(settings_btn);

	UILabel* settings_lbl = (UILabel*)App->ui->AddUILabel(10, 10, "SETTINGS", BLACK, settings_btn);
	settings_btn->button_lbl = settings_lbl;
	labels.PushBack(settings_lbl);
	settings_lbl->interactable = false;


	quit_game_btn = (UIButton*)App->ui->AddUIButton(win_width / 2 - 62, win_height / 2 - 16 + 42 + 42, { 0,32,123,32 }, { 320,105,135,44 }, { 0,64,124,29 }, this);
	buttons.PushBack(quit_game_btn);

	UILabel* quit_game_lbl = (UILabel*)App->ui->AddUILabel(10, 10, "QUIT", BLACK, quit_game_btn);
	quit_game_btn->button_lbl = quit_game_lbl;
	labels.PushBack(quit_game_lbl);
	quit_game_lbl->interactable = false;

	// SETTINGS
	settings_menu = (UIImage*)App->ui->AddUIImage(42, win_height / 2 - 25, { 266,199,192,130 }, this);
	settings_menu->interactable = false;
	settings_menu->invisible = true;

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

	if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN) {
		bool isAnyButtonFocused = false;
		for (int i = 0; i < buttons.Count(); i++)
		{
			if (buttons[i]->current_state == STATE_FOCUSED) {
				isAnyButtonFocused = true;
				buttons[i]->current_state = STATE_NORMAL;
				buttons[i]->UpdateButtonWithSelfRect(buttons[i]->btn_normal);
				buttons[i]->SetLocalPosition(buttons[i]->GetLocalPosition().x, buttons[i]->GetLocalPosition().y - BUTTON_PUSH_OFFSET);

				if (i + 1 != buttons.Count()) { //is not the final button
					buttons[i + 1]->current_state = STATE_FOCUSED;
					buttons[i + 1]->UpdateButtonWithSelfRect(buttons[i]->btn_focused);
					if (buttons[i + 1]->button_lbl != nullptr)
						buttons[i + 1]->button_lbl->SetLocalPosition(buttons[i + 1]->button_lbl->GetLocalPosition().x + BUTTON_HOVER_OFFSET, buttons[i + 1]->button_lbl->GetLocalPosition().y + BUTTON_HOVER_OFFSET);
					buttons[i + 1]->SetLocalPosition(buttons[i + 1]->GetLocalPosition().x - BUTTON_HOVER_OFFSET, buttons[i + 1]->GetLocalPosition().y - BUTTON_HOVER_OFFSET);
				}
				else {
					buttons[0]->current_state = STATE_FOCUSED;
					buttons[0]->UpdateButtonWithSelfRect(buttons[i]->btn_focused);
					if (buttons[0]->button_lbl != nullptr)
						buttons[0]->button_lbl->SetLocalPosition(buttons[0]->button_lbl->GetLocalPosition().x + BUTTON_HOVER_OFFSET, buttons[0]->button_lbl->GetLocalPosition().y + BUTTON_HOVER_OFFSET);
					buttons[0]->SetLocalPosition(buttons[0]->GetLocalPosition().x - BUTTON_HOVER_OFFSET, buttons[0]->GetLocalPosition().y - BUTTON_HOVER_OFFSET);
				}
				break;
			}

		}

		if (!isAnyButtonFocused && buttons.Count() > 0) {
			buttons[0]->current_state = STATE_FOCUSED;
			buttons[0]->UpdateButtonWithSelfRect(buttons[0]->btn_focused);
			if (buttons[0]->button_lbl != nullptr)
				buttons[0]->button_lbl->SetLocalPosition(buttons[0]->button_lbl->GetLocalPosition().x + BUTTON_HOVER_OFFSET, buttons[0]->button_lbl->GetLocalPosition().y + BUTTON_HOVER_OFFSET);
			buttons[0]->SetLocalPosition(buttons[0]->GetLocalPosition().x - BUTTON_HOVER_OFFSET, buttons[0]->GetLocalPosition().y - BUTTON_HOVER_OFFSET);
		}
	}

	return true;
}

// Called each loop iteration
bool j1MainMenu::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || quit_btn_pressed)
		ret = false;

	return ret;
}

// Called before quitting
bool j1MainMenu::CleanUp()
{
	LOG("Freeing main_menu");
	App->ui->DeleteAllUIElements();

	buttons.Clear();
	labels.Clear();

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

void j1MainMenu::OnUITrigger(UIElement* elementTriggered, UI_State ui_state) 
{
	if (elementTriggered->type == BUTTON) 
	{
		UIButton* tmpBtn = (UIButton*)elementTriggered;

		switch (ui_state)
		{
		case STATE_NORMAL: 
		{
			tmpBtn->SetLocalPosition(tmpBtn->GetLocalPosition().x, tmpBtn->GetLocalPosition().y - BUTTON_PUSH_OFFSET);
			if (tmpBtn->last_state == STATE_LEFT_MOUSE_PRESSED &&  App->fadeToBlack->FadeIsOver()) 
			{
				if (tmpBtn == new_game_btn)
					App->fadeToBlack->FadeToBlackBetweenModules(this, App->scene);
				else if (tmpBtn == quit_game_btn)
					quit_btn_pressed = true;
				else if (tmpBtn == settings_btn)
					settings_menu->invisible = false;

			}
				break;
		}
		case STATE_MOUSE_ENTER: 
		{
			for (int i = 0; i < buttons.Count(); i++)
			{
				if (buttons[i]->current_state == STATE_FOCUSED) 
				{
					buttons[i]->current_state = STATE_NORMAL;
					tmpBtn->UpdateButtonWithSelfRect(tmpBtn->btn_normal);
					break;
				}
			}

			tmpBtn->UpdateButtonWithSelfRect(tmpBtn->btn_focused);
			if (tmpBtn->button_lbl != nullptr)
				tmpBtn->button_lbl->SetLocalPosition(tmpBtn->button_lbl->GetLocalPosition().x + BUTTON_HOVER_OFFSET, tmpBtn->button_lbl->GetLocalPosition().y + BUTTON_HOVER_OFFSET);
			tmpBtn->SetLocalPosition(tmpBtn->GetLocalPosition().x - BUTTON_HOVER_OFFSET, tmpBtn->GetLocalPosition().y - BUTTON_HOVER_OFFSET);
			break;
		}
		case STATE_MOUSE_LEAVE:
			tmpBtn->UpdateButtonWithSelfRect(tmpBtn->btn_normal);
			if (tmpBtn->last_state == STATE_LEFT_MOUSE_PRESSED)
			{
				tmpBtn->SetLocalPosition(tmpBtn->GetLocalPosition().x, tmpBtn->GetLocalPosition().y - BUTTON_PUSH_OFFSET);
			}
			else
			{
				if (tmpBtn->button_lbl != nullptr)
					tmpBtn->button_lbl->SetLocalPosition(tmpBtn->button_lbl->GetLocalPosition().x - BUTTON_HOVER_OFFSET, tmpBtn->button_lbl->GetLocalPosition().y - BUTTON_HOVER_OFFSET);
				tmpBtn->SetLocalPosition(tmpBtn->GetLocalPosition().x + BUTTON_HOVER_OFFSET, tmpBtn->GetLocalPosition().y + BUTTON_HOVER_OFFSET);
			}
			break;
		case STATE_LEFT_MOUSE_PRESSED:
			tmpBtn->UpdateButtonWithSelfRect(tmpBtn->btn_pressed);
			if (tmpBtn->button_lbl!=nullptr)
				tmpBtn->button_lbl->SetLocalPosition(tmpBtn->button_lbl->GetLocalPosition().x - BUTTON_HOVER_OFFSET, tmpBtn->button_lbl->GetLocalPosition().y - BUTTON_HOVER_OFFSET);
			tmpBtn->SetLocalPosition(tmpBtn->GetLocalPosition().x + BUTTON_HOVER_OFFSET, tmpBtn->GetLocalPosition().y + BUTTON_HOVER_OFFSET + BUTTON_PUSH_OFFSET);
			break;
		case STATE_NO_DEF:
			break;
		default:
			break;
		}
	}
}