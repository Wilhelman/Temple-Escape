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
#include "UISlider.h"

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
	move_camera = false;

	if (!App->scene->IsGamePaused()) {
		LOG("%s", App->map->sceneName);

		App->map->Load(App->map->sceneName.GetString());

		App->map->LayersSetUp();

		App->map->setAllLogicForMap();

		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);

	}
	LOG("%s", App->map->sceneName);
	
	if (!App->audio->PlayMusic("audio/music/8-bit_Detective.ogg")) {
		//ret = false;
		LOG("Error playing music in j1MainMenu Start");
	}

	uint win_width = 0u, win_height = 0u;
	App->win->GetWindowSize(win_width, win_height);
	win_width /= App->win->GetScale();
	win_height /= App->win->GetScale();

	// MAIN MENU BUTTONS
	

	new_game_btn = (UIButton*)App->ui->AddUIButton(win_width / 2 - 62, win_height / 2 - 25 + 250, { 0,0,123,32 }, { 0,61,135,44 }, { 0,32,124,29 }, this);
	buttons.PushBack(new_game_btn);
	UILabel* new_game_lbl = (UILabel*)App->ui->AddUILabel(25,7, "NEW GAME", BLACK, new_game_btn);
	new_game_btn->button_lbl = new_game_lbl;
	labels.PushBack(new_game_lbl);
	new_game_lbl->interactable = false;

	settings_btn = (UIButton*)App->ui->AddUIButton(300, 215 + 250, { 0,153,28,32 }, { 62,169,40,42 }, { 28,153,28,29 }, this);
	buttons.PushBack(settings_btn);

	quit_game_btn = (UIButton*)App->ui->AddUIButton(10, 215 + 250, { 0,105,28,32 }, { 61,122,40,42 }, { 28,105,28,29 }, this);
	buttons.PushBack(quit_game_btn);

	// SETTINGS
	int tmp_x = 0;
	int tmp_y = 0;
	tmp_x = GetPointToCenter(293, 231, win_width, win_height).x;
	tmp_y = GetPointToCenter(293, 231, win_width, win_height).y;
	settings_menu = (UIImage*)App->ui->AddUIImage(tmp_x, tmp_y, { 135, 0, 293, 231 }, this);
	settings_elements.PushBack(settings_menu);

	close_settings_btn = (UIButton*)App->ui->AddUIButton(295, 20, { 0,137,14,16 }, { 105,130,25,28 }, { 14,137,14,14 }, this);
	settings_elements.PushBack(close_settings_btn);

	

	// SLIDER SETTINGS

	music_volume_slider_lbl = (UILabel*)App->ui->AddUILabel(290, 50, "0%%", GREY);
	settings_elements.PushBack(music_volume_slider_lbl);
	fx_volume_slider_lbl = (UILabel*)App->ui->AddUILabel(290, 100, "0%%", GREY);
	settings_elements.PushBack(fx_volume_slider_lbl);

	music_volume_slider = (UISlider*)App->ui->AddUISlider(150, 50, { 0, 239, 130, 18 }, this);
	settings_elements.PushBack(music_volume_slider);

	music_slider_btn = (UIButton*)App->ui->AddUIButton(50, 0, { 16,185,16,16 }, { 0,201,28,28 }, { 0,185,16,14 }, this, music_volume_slider);
	music_slider_btn->draggable = true;
	music_volume_slider->SetSliderButtons(music_slider_btn);
	settings_elements.PushBack(music_slider_btn);

	fx_volume_slider = (UISlider*)App->ui->AddUISlider(150, 100, { 0, 239, 130, 18 }, this);
	settings_elements.PushBack(fx_volume_slider);

	fx_slider_btn = (UIButton*)App->ui->AddUIButton(50, 50, { 16,185,16,16 }, { 0,201,28,28 }, { 0,185,16,14 }, this, music_volume_slider);
	fx_slider_btn->draggable = true;
	fx_volume_slider->SetSliderButtons(fx_slider_btn);
	settings_elements.PushBack(fx_slider_btn);

	music_volume_lbl = (UILabel*)App->ui->AddUILabel(32, 49, "Music volume:", BLACK);
	settings_elements.PushBack(music_volume_lbl);

	fx_volume_lbl = (UILabel*)App->ui->AddUILabel(32, 100, "FX volume:", BLACK);
	settings_elements.PushBack(fx_volume_lbl);

	for (int i = 0; i < settings_elements.Count(); i++)
	{
		settings_elements[i]->interactable = false;
		settings_elements[i]->invisible = true;
	}
	for (int i = 0; i < buttons.Count(); i++)
	{
		buttons[i]->interactable = false;
		buttons[i]->invisible = true;
	}
	for (int i = 0; i < labels.Count(); i++)
	{
		labels[i]->interactable = false;
		labels[i]->invisible = true;
	}

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

	if (music_volume_slider != nullptr && (music_volume_slider->GetSliderButton()->current_state != STATE_MOUSE_ENTER && fx_volume_slider->GetSliderButton()->current_state != STATE_MOUSE_ENTER))
	{
		music_volume_slider->SetSliderValue(music_volume_slider->GetSliderButton()->GetLocalPosition().x);
		music_volume_slider_lbl->SetTextFromNum(music_volume_slider->GetSliderValue());
		Mix_VolumeMusic(128*music_volume_slider->GetSliderValue()/100);

		fx_volume_slider->SetSliderValue(fx_volume_slider->GetSliderButton()->GetLocalPosition().x);
		fx_volume_slider_lbl->SetTextFromNum(fx_volume_slider->GetSliderValue());
		Mix_Volume(-1, 128 * fx_volume_slider->GetSliderValue() / 100);
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && !move_camera)
		move_camera = true;

	if (move_camera && App->render->camera.x > -2050)
		App->render->camera.x -= 10;
	else if (App->render->camera.x == -2050)
	{
		for (int i = 0; i < buttons.Count(); i++)
			buttons[i]->invisible = false;

		for (int i = 0; i < labels.Count(); i++)
			labels[i]->invisible = false;

		if (new_game_btn->GetLocalPosition().y > 103 && !new_game_btn->interactable)
			new_game_btn->SetLocalPosition(new_game_btn->GetLocalPosition().x, new_game_btn->GetLocalPosition().y - 10);
		if (settings_btn->GetLocalPosition().y > 215 && !settings_btn->interactable)
			settings_btn->SetLocalPosition(settings_btn->GetLocalPosition().x, settings_btn->GetLocalPosition().y - 10);
		if (quit_game_btn->GetLocalPosition().y > 215 && !quit_game_btn->interactable)
			quit_game_btn->SetLocalPosition(quit_game_btn->GetLocalPosition().x, quit_game_btn->GetLocalPosition().y - 10);
	}

	if (new_game_btn->GetLocalPosition().y == 103
		&& settings_btn->GetLocalPosition().y == 215
		&& settings_btn->GetLocalPosition().y == 215)
	{
		for (int i = 0; i < buttons.Count(); i++)
			buttons[i]->interactable = true;
	}


	App->map->Draw();

	return true;
}

// Called each loop iteration
bool j1MainMenu::PostUpdate()
{
	bool ret = true;

	if (quit_btn_pressed)
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
	settings_elements.Clear();

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
		case STATE_LEFT_MOUSE_RELEASED:
		case STATE_NORMAL: 
		{
			tmpBtn->SetLocalPosition(tmpBtn->GetLocalPosition().x, tmpBtn->GetLocalPosition().y - BUTTON_PUSH_OFFSET);
			if (tmpBtn->last_state == STATE_LEFT_MOUSE_PRESSED &&  App->fadeToBlack->FadeIsOver())
			{
				if (tmpBtn == new_game_btn)
					App->fadeToBlack->FadeToBlackBetweenModules(this, App->scene,1.0f);
				else if (tmpBtn == quit_game_btn)
					quit_btn_pressed = true;
				else if (tmpBtn == settings_btn)
				{
					for (int i = 0; i < settings_elements.Count(); i++)
					{
						if (settings_elements[i]->type == LABEL)
						{
							settings_elements[i]->interactable = false;
							settings_elements[i]->invisible = false;
						}
						else
						{
							settings_elements[i]->interactable = true;
							settings_elements[i]->invisible = false;
						}
						
					}
				}			
				else if (tmpBtn == close_settings_btn)
				{
					for (int i = 0; i < settings_elements.Count(); i++)
					{
						if (settings_elements[i]->type == LABEL)
						{
							settings_elements[i]->interactable = false;
							settings_elements[i]->invisible = true;
						}
						else
						{
							settings_elements[i]->interactable = false;
							settings_elements[i]->invisible = true;
						}

					}
				}
			}
			tmpBtn->UpdateButtonWithSelfRect(tmpBtn->btn_normal);
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

iPoint j1MainMenu::GetPointToCenter(int w_to_center, int h_to_center, int w_ref, int h_ref)
{

	return iPoint((w_ref - w_to_center) / 2, (h_ref - h_to_center) / 2);
}