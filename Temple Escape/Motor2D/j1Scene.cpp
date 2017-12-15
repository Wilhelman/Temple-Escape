#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Collider.h"
#include "j1FadeToBlack.h"
#include "j1Pathfinding.h"
#include "j1MainMenu.h"
#include "Player.h"
#include "j1UI.h"

#include "UIImage.h"
#include "UIButton.h"

#include "j1Entities.h"

#define SCORE_UI {0,419,10,14}

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available

bool j1Scene::Awake()
{

	LOG("Loading Scene");
	bool ret = true;
	
	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	bool ret = true;

	pugi::xml_document	config_file;
	pugi::xml_node* node = &App->LoadConfig(config_file); 
	node = &node->child("map").child("scene");

	//read animation from node
	for (pugi::xml_node animations = node->child("animations").child("animation"); animations && ret; animations = animations.next_sibling("animation"))
	{
		p2SString tmp(animations.attribute("name").as_string());

		if (tmp == "heart_reward_anim")
			LoadSceneAnimation(animations, &heart_reward_anim);
		if (tmp == "coin_reward_anim")
			LoadSceneAnimation(animations, &coin_reward_anim);
	}

	if (!App->audio->PlayMusic("audio/music/arcade_funk.ogg")) 
	{
		//ret = false;
		LOG("Error playing music in j1Scene Start");
	}
	
	atlas_tex = App->ui->GetAtlas();

	// PAUSE WINDOW
	uint win_width = 0u, win_height = 0u;
	App->win->GetWindowSize(win_width, win_height);
	win_width /= App->win->GetScale();
	win_height /= App->win->GetScale();

	player_lives = (UIImage*)App->ui->AddUIImage(5, 5, PLAYER_5_LIVE, this);
	player_lives->interactable = false;
	player_lives->invisible = false;

	coin_ui = (UIImage*)App->ui->AddUIImage(318, 8, SCORE_UI, this);
	coin_ui->interactable = false;
	coin_ui->invisible = false;

	score_lbl = (UILabel*)App->ui->AddUILabel(300, 7, "0", WHITE, 20);
	score_lbl->interactable = false;
	score_lbl->invisible = false;

	pause_menu = (UIImage*)App->ui->AddUIImage(0, 0, { 135, 231, 342, 256 }, this);
	pause_menu->interactable = false;
	pause_menu->invisible = true;

	continue_btn = (UIButton*)App->ui->AddUIButton(win_width / 2 - 62, win_height / 2 - 16 - 80, { 0,0,123,32 }, { 0,61,135,44 }, { 0,32,124,29 }, this, pause_menu);
	buttons.PushBack(continue_btn);
	continue_btn->interactable = false;
	continue_btn->invisible = true;
	UILabel* continue_lbl = (UILabel*)App->ui->AddUILabel(28, 7, "CONTINUE", BLACK, 20, continue_btn);
	continue_btn->button_lbl = continue_lbl;
	continue_lbl->interactable = false;
	continue_lbl->invisible = true;

	save_game_btn = (UIButton*)App->ui->AddUIButton(win_width / 2 - 62, win_height / 2 - 16 - 40, { 0,0,123,32 }, { 0,61,135,44 }, { 0,32,124,29 }, this, pause_menu);
	buttons.PushBack(save_game_btn);
	save_game_btn->interactable = false;
	save_game_btn->invisible = true;
	UILabel* save_game_lbl = (UILabel*)App->ui->AddUILabel(43, 7, "SAVE", BLACK, 20, save_game_btn);
	save_game_btn->button_lbl = save_game_lbl;
	save_game_lbl->interactable = false;
	save_game_lbl->invisible = true;

	load_game_btn = (UIButton*)App->ui->AddUIButton(win_width / 2 - 62, win_height / 2 - 16, { 0,0,123,32 }, { 0,61,135,44 }, { 0,32,124,29 }, this, pause_menu);
	buttons.PushBack(load_game_btn);
	load_game_btn->interactable = false;
	load_game_btn->invisible = true;
	UILabel* load_game_lbl = (UILabel*)App->ui->AddUILabel(43, 7, "LOAD", BLACK, 20, load_game_btn);
	load_game_btn->button_lbl = load_game_lbl;
	load_game_lbl->interactable = false;
	load_game_lbl->invisible = true;

	main_menu_btn = (UIButton*)App->ui->AddUIButton(win_width / 2 - 62, win_height / 2 - 16 + 40, { 0,0,123,32 }, { 0,61,135,44 }, { 0,32,124,29 }, this, pause_menu);
	buttons.PushBack(main_menu_btn);
	main_menu_btn->interactable = false;
	main_menu_btn->invisible = true;
	UILabel* main_menu_lbl = (UILabel*)App->ui->AddUILabel(20, 7, "MAIN MENU", BLACK, 20, main_menu_btn);
	main_menu_btn->button_lbl = main_menu_lbl;
	main_menu_lbl->interactable = false;
	main_menu_lbl->invisible = true;

	quit_btn = (UIButton*)App->ui->AddUIButton(win_width / 2 - 62, win_height / 2 - 16 + 80, { 0,0,123,32 }, { 0,61,135,44 }, { 0,32,124,29 }, this,pause_menu);
	buttons.PushBack(quit_btn);
	quit_btn->interactable = false;
	quit_btn->invisible = true;
	UILabel* quit_lbl = (UILabel*)App->ui->AddUILabel(43, 7, "QUIT", BLACK, 20, quit_btn);
	quit_btn->button_lbl = quit_lbl;
	quit_lbl->interactable = false;
	quit_lbl->invisible = true;

	return ret;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	//DEBUG
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN && App->fadeToBlack->FadeIsOver()) 
	{
		if (App->map->sceneName != "lvl1.tmx") 
		{
			App->fadeToBlack->F1 = true;
			App->fadeToBlack->FadeToKnowBlack("lvl1.tmx");
		}
		else 
			putPlayerToSpawn();
	}

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) 
		putPlayerToSpawn();

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN && App->fadeToBlack->FadeIsOver() && !App->entities->GetPlayer()->isDead)
		App->SaveGame();
		
	if(App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN&& App->fadeToBlack->FadeIsOver())
		App->LoadGame();

	//END DEBUG

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN&& App->fadeToBlack->FadeIsOver()) 
	{
		pause_menu->invisible = !pause_menu->invisible;
		pause_menu->interactable = !pause_menu->interactable;

		for (int i = 0; i < buttons.Count(); i++)
		{
			buttons[i]->interactable = !buttons[i]->interactable;
			buttons[i]->invisible = !buttons[i]->invisible;
			buttons[i]->button_lbl->invisible = !buttons[i]->button_lbl->invisible;
		}

		paused = !paused;
	}

	switch (App->entities->GetPlayer()->p_lives)
	{
	case 6:
		player_lives->UpdateImageWithCoords(PLAYER_6_LIVE);
		break;
	case 5:
		player_lives->UpdateImageWithCoords(PLAYER_5_LIVE);
		break;
	case 4:
		player_lives->UpdateImageWithCoords(PLAYER_4_LIVE);
		break;
	case 3:
		player_lives->UpdateImageWithCoords(PLAYER_3_LIVE);
		break;
	case 2:
		player_lives->UpdateImageWithCoords(PLAYER_2_LIVE);
		break;
	case 1:
		player_lives->UpdateImageWithCoords(PLAYER_1_LIVE);
		break;
	case 0:
		App->fadeToBlack->FadeToBlackBetweenModules(this, App->main_menu, 1.0f);
		break;
	default:
		break;
	}

	if (App->entities->GetPlayer()->score > 0)
		score_lbl->SetTextFromNum(App->entities->GetPlayer()->score);



	App->map->Draw();

	if (App->entities->GetPlayer()->score > 0 && App->entities->GetPlayer()->score % 10 == 0 /*&& (!heart_reward_anim.Finished() && !coin_reward_anim.Finished())*/)
	{
		App->render->Blit((SDL_Texture*)atlas_tex, App->entities->GetPlayer()->position.x + App->entities->GetPlayer()->current_frame.w / 2 - heart_reward_anim.GetCurrentFrame().w / 2, App->entities->GetPlayer()->position.y - App->entities->GetPlayer()->current_frame.h - heart_reward_anim.GetCurrentFrame().h, &heart_reward_anim.GetCurrentFrame());
		LOG("%f %f", App->entities->GetPlayer()->position.x, App->entities->GetPlayer()->position.y);
		App->render->Blit((SDL_Texture*)atlas_tex, App->entities->GetPlayer()->position.x + 100, App->entities->GetPlayer()->position.y - 130, &coin_reward_anim.GetCurrentFrame());
		coin_ui->invisible = true;
	}
	else
		coin_ui->invisible = false;
		

	int m_x; int m_y;
	App->input->GetMousePosition(m_x, m_y);
	iPoint p = App->map->WorldToMap(m_x, m_y);

	p2SString title("TEMPLE ESCAPE - Map:%dx%d Tiles:%dx%d Tilesets:%d || Coords in Map: X->%i Y->%i || Coords in World: X->%i Y->%i",
					App->map->data.width, App->map->data.height,
					App->map->data.tile_width, App->map->data.tile_height,
					App->map->data.tilesets.count(),p.x,p.y,m_x,m_y);

	//App->win->SetTitle(title.GetString()); // TODO: remove this if not needed and above too

	if (App->entities->GetPlayer()->reachedEnd && App->fadeToBlack->FadeIsOver()) 
	{
		App->entities->GetPlayer()->reachedEnd = false;
		App->fadeToBlack->FadeToBlack();
	}

	return true;
}

void j1Scene::putPlayerToSpawn() 
{
	App->entities->GetPlayer()->position.x = App->map->spawn.x;
	App->entities->GetPlayer()->position.y = App->map->spawn.y;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if (quit_btn_pressed)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");
	App->ui->DeleteAllUIElements();

	buttons.Clear();
	hud_elements.Clear();

	paused = false;

	return true;
}

bool j1Scene::Load(pugi::xml_node& load) 
{
	bool ret = true;

	if (!load.child("currentLvl").empty()) 
	{
		p2SString lvlToLoad = (p2SString)load.child("currentLvl").attribute("name").as_string();

		if (App->map->sceneName != lvlToLoad && App->fadeToBlack->FadeIsOver()) 
		{
			App->fadeToBlack->lvlName = lvlToLoad;
		}
	}

	return ret;
}

bool j1Scene::Save(pugi::xml_node& save) const 
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

void j1Scene::OnUITrigger(UIElement* elementTriggered, UI_State ui_state) {
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
				if (tmpBtn == main_menu_btn)
					App->fadeToBlack->FadeToBlackBetweenModules(this, App->main_menu, 1.0f);
				else if (tmpBtn == quit_btn)
					quit_btn_pressed = true;
				else if (tmpBtn == continue_btn)
				{
					pause_menu->invisible = !pause_menu->invisible;
					pause_menu->interactable = !pause_menu->interactable;
					for (int i = 0; i < buttons.Count(); i++)
					{
						buttons[i]->interactable = !buttons[i]->interactable;
						buttons[i]->invisible = !buttons[i]->invisible;
						buttons[i]->button_lbl->invisible = !buttons[i]->button_lbl->invisible;
					}
					paused = !paused;
				}
				else if (tmpBtn == load_game_btn)
				{
					App->LoadGame();
				}
				else if (tmpBtn == save_game_btn)
				{
					App->SaveGame();
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
}

bool j1Scene::IsGamePaused() 
{
	return paused;
}

void j1Scene::LoadSceneAnimation(pugi::xml_node animation_node, p2Animation * animation)
{
	bool ret = true;

	for (pugi::xml_node frame = animation_node.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
		animation->PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

	animation->speed = animation_node.attribute("speed").as_float();
	animation->loop = animation_node.attribute("loop").as_bool();
}