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
#include "j1Language.h"
#include "j1Credits.h"

#include "UIImage.h"
#include "UIButton.h"
#include "UILabel.h"
#include "UISlider.h"
#include "UICheckbox.h"

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
	settings_up = false;
	settings_down = false;
	capped_checkbox = true;

	camera_limit = -2050;
	camera_step_move = 20;
	game_btn_original_pos = 103;
	settings_menu_original_pos = 2;
	game_btn_final_pos = 93;
	settings_menu_final_pos = 262;

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

	press_space_lbl = (UILabel*)App->ui->AddUILabel(122, 216, "Press SPACE to continue" , YELLOW, 10, 0);
	press_space_lbl->interactable = false;
	press_space_lbl->invisible = false;


	new_game_btn = (UIButton*)App->ui->AddUIButton(win_width / 2 - 62, win_height / 2 - 25 + 250, { 0,0,123,32 }, { 0,61,135,44 }, { 0,32,124,29 }, this);
	buttons.PushBack(new_game_btn);
	new_game_lbl = (UILabel*)App->ui->AddUILabel(25,7, App->languages->GetDictionary().new_game_btn , BLACK, 20,0, new_game_btn);
	new_game_btn->button_lbl = new_game_lbl;
	labels.PushBack(new_game_lbl);
	new_game_lbl->interactable = false;

	credits_btn = (UIButton*)App->ui->AddUIButton(win_width / 2 - 62, win_height / 2 - 25 + 250 + 100, { 0,0,123,32 }, { 0,61,135,44 }, { 0,32,124,29 }, this);
	buttons.PushBack(credits_btn);
	credits_btn_lbl = (UILabel*)App->ui->AddUILabel(25, 7, App->languages->GetDictionary().credits_btn, BLACK, 20,0, credits_btn);
	credits_btn->button_lbl = credits_btn_lbl;
	labels.PushBack(credits_btn_lbl);
	credits_btn_lbl->interactable = false;

	settings_btn = (UIButton*)App->ui->AddUIButton(300, 228 + 250, { 0,153,28,32 }, { 62,169,40,42 }, { 28,153,28,29 }, this);
	buttons.PushBack(settings_btn);

	quit_game_btn = (UIButton*)App->ui->AddUIButton(10, 228 + 250, { 0,105,28,32 }, { 61,122,40,42 }, { 28,105,28,29 }, this);
	buttons.PushBack(quit_game_btn);

	pugi::xml_document	save_file;

	if (App->LoadSave(save_file)) 
	{
		continue_btn = (UIButton*)App->ui->AddUIButton(win_width / 2 - 62, win_height / 2 - 25 + 50 + 250, { 0,0,123,32 }, { 0,61,135,44 }, { 0,32,124,29 }, this);
		buttons.PushBack(continue_btn);
		continue_lbl = (UILabel*)App->ui->AddUILabel(28, 7, App->languages->GetDictionary().continue_btn, BLACK, 20,0, continue_btn);
		continue_btn->button_lbl = continue_lbl;
		labels.PushBack(continue_lbl);
		continue_lbl->interactable = false;
	}

	// SETTINGS
	int tmp_x = 0;
	int tmp_y = 0;
	tmp_x = GetPointToCenter(293, 231, win_width, win_height).x;
	tmp_y = GetPointToCenter(293, 231, win_width, win_height).y;

	settings_menu = (UIImage*)App->ui->AddUIImage(tmp_x, tmp_y + 250, { 135, 0, 293, 231 }, this);
	settings_elements.PushBack(settings_menu);

	close_settings_btn = (UIButton*)App->ui->AddUIButton(295, 20 + 250, { 0,137,14,16 }, { 105,130,25,28 }, { 14,137,14,14 }, this);
	settings_elements.PushBack(close_settings_btn);

	cap_checkbox = (UICheckBox*)App->ui->AddUICheckBox(132, 140, { 0, 273, 14, 16 }, { 14, 273, 14, 16 }, { 0, 289, 14, 14 }, { 14, 289, 14, 14 }, { 28, 273, 26, 28 }, { 54, 273, 26, 28 }, this, settings_menu);
	settings_elements.PushBack(cap_checkbox);

	cap_lbl = (UILabel*)App->ui->AddUILabel(35, 150 + 250, App->languages->GetDictionary().cap_to, BLACK, 20);
	settings_elements.PushBack(cap_lbl);

	language_left_btn = (UIButton*)App->ui->AddUIButton(151, 200 + 250, { 0,303,14,16 }, { 28,301,26,28 }, { 14,303,14,14 }, this);
	settings_elements.PushBack(language_left_btn);

	language_right_btn = (UIButton*)App->ui->AddUIButton(250, 200 + 250, { 14,319,14,16 }, { 54,301,26,28 }, { 0,319,14,14 }, this);
	settings_elements.PushBack(language_right_btn);

	language_lbl = (UILabel*)App->ui->AddUILabel(35, 200 + 250, App->languages->GetDictionary().language_option, BLACK, 20);
	settings_elements.PushBack(language_lbl);

	selected_language_lbl = (UILabel*)App->ui->AddUILabel(195, 200 + 250, App->languages->current_language, BLACK, 20);
	settings_elements.PushBack(selected_language_lbl);

	
	// SLIDER SETTINGS

	pugi::xml_document	config_file;
	pugi::xml_node* node = &App->LoadConfig(config_file);

	music_volume_slider_lbl = (UILabel*)App->ui->AddUILabel(287, 37 + 250, "0%%", GREY,10);
	settings_elements.PushBack(music_volume_slider_lbl);
	fx_volume_slider_lbl = (UILabel*)App->ui->AddUILabel(287, 87 + 250, "0%%", GREY,10);
	settings_elements.PushBack(fx_volume_slider_lbl);
	
	music_volume_slider = (UISlider*)App->ui->AddUISlider(172, 50 + 250, { 0, 239, 130, 18 }, this);
	settings_elements.PushBack(music_volume_slider);

	music_slider_btn = (UIButton*)App->ui->AddUIButton(50, 0, { 16,185,16,16 }, { 0,201,28,28 }, { 0,185,16,14 }, this, music_volume_slider);
	music_slider_btn->draggable = true;
	music_volume_slider->SetSliderButtons(music_slider_btn);
	music_volume_slider->SetSliderValueStart(node->child("music").attribute("music").as_int());
	settings_elements.PushBack(music_slider_btn);

	fx_volume_slider = (UISlider*)App->ui->AddUISlider(172, 100 + 250, { 0, 239, 130, 18 }, this);
	settings_elements.PushBack(fx_volume_slider);

	fx_slider_btn = (UIButton*)App->ui->AddUIButton(50, 0, { 16,185,16,16 }, { 0,201,28,28 }, { 0,185,16,14 }, this, fx_volume_slider);
	fx_slider_btn->draggable = true;
	fx_volume_slider->SetSliderButtons(fx_slider_btn);
	fx_volume_slider->SetSliderValueStart(node->child("music").attribute("fx").as_int());
	settings_elements.PushBack(fx_slider_btn);

	node = nullptr;

	music_volume_lbl = (UILabel*)App->ui->AddUILabel(35, 49 + 250, App->languages->GetDictionary().music_volume, BLACK,20);
	settings_elements.PushBack(music_volume_lbl);

	fx_volume_lbl = (UILabel*)App->ui->AddUILabel(35, 100 + 250, App->languages->GetDictionary().fx_volume, BLACK, 20);
	settings_elements.PushBack(fx_volume_lbl);

	

	for (int i = 0; i < settings_elements.Count(); i++)
	{
		settings_elements[i]->interactable = false;
		settings_elements[i]->invisible = false;
	}
	for (int i = 0; i < buttons.Count(); i++)
	{
		buttons[i]->interactable = false;
		buttons[i]->invisible = true;
		if (buttons[i]->button_lbl != nullptr)
			buttons[i]->CenterTextInButton();
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
	// Sliders % control
	if (music_volume_slider != nullptr && (music_volume_slider->GetSliderButton()->current_state != STATE_MOUSE_ENTER && fx_volume_slider->GetSliderButton()->current_state != STATE_MOUSE_ENTER))
	{
		music_volume_slider->SetSliderValue(music_volume_slider->GetSliderButton()->GetLocalPosition().x);
		music_volume_slider_lbl->SetTextFromNum(music_volume_slider->GetSliderValue(),true);
		Mix_VolumeMusic(128*music_volume_slider->GetSliderValue()/100);

		fx_volume_slider->SetSliderValue(fx_volume_slider->GetSliderButton()->GetLocalPosition().x);
		fx_volume_slider_lbl->SetTextFromNum(fx_volume_slider->GetSliderValue(), true);

		if(App->audio->active)
			Mix_Volume(-1, 128 * fx_volume_slider->GetSliderValue() / 100);
	}

	// Settings animations
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && !move_camera)
	{
		press_space_lbl->invisible = true;
		move_camera = true;
	}
		
	if (move_camera && App->render->camera.x > camera_limit)
		App->render->camera.x -= camera_step_move;
	else if (App->render->camera.x == camera_limit)
	{
		for (int i = 0; i < buttons.Count(); i++)
			buttons[i]->invisible = false;

		for (int i = 0; i < labels.Count(); i++)
			labels[i]->invisible = false;

		if (new_game_btn->GetLocalPosition().y > game_btn_original_pos && !new_game_btn->interactable)
		{
			for (int i = 0; i < buttons.Count(); i++)
				buttons[i]->SetLocalPosition(buttons[i]->GetLocalPosition().x, buttons[i]->GetLocalPosition().y - camera_step_move);
		}

		if (settings_menu->GetLocalPosition().y > settings_menu_original_pos && settings_up)
		{
			for (int i = 0; i < settings_elements.Count(); i++)
			{
				if (settings_elements[i]->parent == nullptr)
					settings_elements[i]->SetLocalPosition(settings_elements[i]->GetLocalPosition().x, settings_elements[i]->GetLocalPosition().y - camera_step_move);
			}
		}
		else if (settings_menu->GetLocalPosition().y < settings_menu_final_pos && settings_down)
		{
			for (int i = 0; i < settings_elements.Count(); i++)
			{
				if (settings_elements[i]->parent == nullptr)
					settings_elements[i]->SetLocalPosition(settings_elements[i]->GetLocalPosition().x, settings_elements[i]->GetLocalPosition().y + camera_step_move);
			}
		}
	}

	if (new_game_btn->GetLocalPosition().y == game_btn_final_pos)
	{
		for (int i = 0; i < buttons.Count(); i++)
			buttons[i]->interactable = true;
	}

	if (settings_menu->GetLocalPosition().y == settings_menu_original_pos && !settings_down)
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
				settings_elements[i]->invisible = false;
				settings_elements[i]->interactable = true;
			}
		}	
	}
	//-------

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

	new_game_btn = nullptr;
	continue_btn = nullptr;
	settings_btn = nullptr;
	quit_game_btn = nullptr;
	close_settings_btn = nullptr;
	music_slider_btn = nullptr;
	fx_slider_btn = nullptr;
	language_right_btn = nullptr;
	language_left_btn = nullptr;
	credits_btn = nullptr;

	settings_menu = nullptr;
	music_volume_slider = nullptr;
	fx_volume_slider = nullptr;

	music_volume_slider_lbl = nullptr;
	music_volume_lbl = nullptr;
	fx_volume_slider_lbl = nullptr;
	fx_volume_lbl = nullptr;
	cap_lbl = nullptr;
	language_lbl = nullptr;
	selected_language_lbl = nullptr;
	new_game_lbl = nullptr;
	continue_lbl = nullptr;
	credits_btn_lbl = nullptr;

	cap_checkbox = nullptr;

	press_space_lbl = nullptr;

	for (int i = 0; i < buttons.Count(); i++)
		buttons[i] = nullptr;
	
	buttons.Clear();

	for (int i = 0; i < labels.Count(); i++)
		labels[i] = nullptr;
	
	labels.Clear();

	for (int i = 0; i < settings_elements.Count(); i++)
		settings_elements[i] = nullptr;
	
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
			App->fadeToBlack->FadeToBlackBetweenModules(this, App->scene, 1.0f, true);
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
				if (tmpBtn == new_game_btn) {
					App->fadeToBlack->FadeToBlackBetweenModules(this, App->scene, 1.0f);
				}
				else if (tmpBtn == continue_btn) {
					App->LoadGame();
				}
				else if (tmpBtn == quit_game_btn)
					quit_btn_pressed = true;
				else if (tmpBtn == settings_btn)
				{
					for (int i = 0; i < settings_elements.Count(); i++)
						settings_elements[i]->interactable = false;

					settings_up = true;
					settings_down = false;
					settings_menu->invisible = false;
				}
				else if (tmpBtn == close_settings_btn)
				{
					settings_down = true;
					settings_up = false;

					for (int i = 0; i < settings_elements.Count(); i++)
					{
						settings_elements[i]->interactable = false;

						//if (settings_elements[i]->type != IMAGE)
							//settings_elements[i]->invisible = true;

					}

					pugi::xml_document	config_file;
					pugi::xml_node* node = &App->LoadConfig(config_file); 
					node->child("language").attribute("current").set_value(App->languages->current_language.GetString());
					node->child("music").attribute("fx").set_value(fx_volume_slider->GetSliderValue());
					node->child("music").attribute("music").set_value(music_volume_slider->GetSliderValue());

					config_file.save_file("config.xml");
				}
				else if (tmpBtn == language_left_btn) {
					for (int i = 0; i < App->languages->posible_languages.Count(); i++)
					{
						if (App->languages->posible_languages[i] == selected_language_lbl->text && i != 0) {
							selected_language_lbl->SetText(App->languages->posible_languages[i - 1]);
							App->languages->ChangeCurrentLanguage(App->languages->posible_languages[i - 1]);
							break;
						}
						else if (App->languages->posible_languages[i] == selected_language_lbl->text && i == 0) {
							selected_language_lbl->SetText(App->languages->posible_languages[App->languages->posible_languages.Count() - 1]);
							App->languages->ChangeCurrentLanguage(App->languages->posible_languages[App->languages->posible_languages.Count() - 1]);
							break;
						}
					}
					ResetTextToLabels();
				}
				else if (tmpBtn == language_right_btn) {
					for (int i = 0; i < App->languages->posible_languages.Count(); i++)
					{
						if (App->languages->posible_languages[i] == selected_language_lbl->text && i != App->languages->posible_languages.Count() - 1) {
							selected_language_lbl->SetText(App->languages->posible_languages[i + 1]);
							App->languages->ChangeCurrentLanguage(App->languages->posible_languages[i + 1]);
							break;
						}
						else if (App->languages->posible_languages[i] == selected_language_lbl->text && i == App->languages->posible_languages.Count() - 1) {
							selected_language_lbl->SetText(App->languages->posible_languages[0]);
							App->languages->ChangeCurrentLanguage(App->languages->posible_languages[0]);
							break;
						}
					}
					ResetTextToLabels();
				}
				else if (tmpBtn == credits_btn) {
					App->fadeToBlack->FadeToBlackBetweenModules(this, App->credits, 1.0f);
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
			if (tmpBtn->button_lbl != nullptr)
				tmpBtn->button_lbl->SetLocalPosition(tmpBtn->button_lbl->GetLocalPosition().x - BUTTON_HOVER_OFFSET, tmpBtn->button_lbl->GetLocalPosition().y - BUTTON_HOVER_OFFSET);
			tmpBtn->SetLocalPosition(tmpBtn->GetLocalPosition().x + BUTTON_HOVER_OFFSET, tmpBtn->GetLocalPosition().y + BUTTON_HOVER_OFFSET + BUTTON_PUSH_OFFSET);
			break;
		case STATE_NO_DEF:
			break;
		default:
			break;
		}
	}

	if (elementTriggered->type == CHECK_BOX)
	{
		UICheckBox* tmpCb = (UICheckBox*)elementTriggered;

		switch (ui_state)
		{
		case STATE_LEFT_MOUSE_RELEASED:
		case STATE_NORMAL:
		{
			tmpCb->SetLocalPosition(tmpCb->GetLocalPosition().x, tmpCb->GetLocalPosition().y - BUTTON_PUSH_OFFSET);
			if (tmpCb->last_state == STATE_LEFT_MOUSE_PRESSED &&  App->fadeToBlack->FadeIsOver())
			{
				if (tmpCb->check_box_state)
					tmpCb->UpdateCheckBoxWithSelfRect(tmpCb->cb_tick_normal);
				else
					tmpCb->UpdateCheckBoxWithSelfRect(tmpCb->cb_no_tick_normal);
				break;
			}
		case STATE_MOUSE_ENTER:
		{

			if (tmpCb->current_state == STATE_FOCUSED)
			{
				tmpCb->current_state = STATE_NORMAL;

				if (tmpCb->check_box_state)
					tmpCb->UpdateCheckBoxWithSelfRect(tmpCb->cb_tick_focus);
				else
					tmpCb->UpdateCheckBoxWithSelfRect(tmpCb->cb_no_tick_focus);

				break;
			}
			if (tmpCb->check_box_state)
				tmpCb->UpdateCheckBoxWithSelfRect(tmpCb->cb_tick_focus);
			else
				tmpCb->UpdateCheckBoxWithSelfRect(tmpCb->cb_no_tick_focus);

			tmpCb->SetLocalPosition(tmpCb->GetLocalPosition().x - BUTTON_HOVER_OFFSET, tmpCb->GetLocalPosition().y - BUTTON_HOVER_OFFSET);

			break;
		}
		case STATE_MOUSE_LEAVE:

			if (tmpCb->check_box_state)
				tmpCb->UpdateCheckBoxWithSelfRect(tmpCb->cb_tick_normal);
			else
				tmpCb->UpdateCheckBoxWithSelfRect(tmpCb->cb_no_tick_normal);

			if (tmpCb->last_state == STATE_LEFT_MOUSE_PRESSED)
				tmpCb->SetLocalPosition(tmpCb->GetLocalPosition().x, tmpCb->GetLocalPosition().y - BUTTON_PUSH_OFFSET);
			else
				tmpCb->SetLocalPosition(tmpCb->GetLocalPosition().x + BUTTON_HOVER_OFFSET, tmpCb->GetLocalPosition().y + BUTTON_HOVER_OFFSET);

			break;
		case STATE_LEFT_MOUSE_PRESSED:

			if (tmpCb->check_box_state)
			{
				tmpCb->check_box_state = false;
				capped_checkbox = false;
				App->cap_state = false;
			}
			else
			{
				tmpCb->check_box_state = true;
				capped_checkbox = true;
				App->cap_state = true;
			}

			if (tmpCb->check_box_state)
				tmpCb->UpdateCheckBoxWithSelfRect(tmpCb->cb_tick_pressed);
			else
				tmpCb->UpdateCheckBoxWithSelfRect(tmpCb->cb_no_tick_pressed);
			
			tmpCb->SetLocalPosition(tmpCb->GetLocalPosition().x + BUTTON_HOVER_OFFSET, tmpCb->GetLocalPosition().y + BUTTON_HOVER_OFFSET + BUTTON_PUSH_OFFSET);
			break;
		case STATE_NO_DEF:
			break;
		default:
			break;
		}
		}

	}
}

iPoint j1MainMenu::GetPointToCenter(int w_to_center, int h_to_center, int w_ref, int h_ref)
{

	return iPoint((w_ref - w_to_center) / 2, (h_ref - h_to_center) / 2);
}

void j1MainMenu::ResetTextToLabels() 
{
	new_game_lbl->SetText(App->languages->GetDictionary().new_game_btn);
	continue_lbl->SetText(App->languages->GetDictionary().continue_btn);
	credits_btn_lbl->SetText(App->languages->GetDictionary().credits_btn);
	fx_volume_lbl->SetText(App->languages->GetDictionary().fx_volume);
	music_volume_lbl->SetText(App->languages->GetDictionary().music_volume);
	language_lbl->SetText(App->languages->GetDictionary().language_option);
	cap_lbl->SetText(App->languages->GetDictionary().cap_to);

	for (int i = 0; i < buttons.Count(); i++)
	{
		if (buttons[i]->button_lbl != nullptr)
			buttons[i]->CenterTextInButton();
	}
}