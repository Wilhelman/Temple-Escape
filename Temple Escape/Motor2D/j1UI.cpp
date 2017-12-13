#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1UI.h"

#include "UILabel.h"
#include "UIImage.h"
#include "UIButton.h"
#include "UISlider.h"
#include "UICheckbox.h"

#include "UIElement.h"

j1UI::j1UI() : j1Module()
{
	name.create("ui");
}

// Destructor
j1UI::~j1UI()
{}

// Called before render is available
bool j1UI::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");

	return ret;
}

// Called before the first frame
bool j1UI::Start()
{
	atlas = App->tex->Load(atlas_file_name.GetString());


	return true;
}

// Update all guis
bool j1UI::PreUpdate()
{
	for (int i = 0; i < ui_elements.Count(); i++)
		if (ui_elements.At(i) != nullptr) ui_elements[i]->Update();

	return true;
}

bool j1UI::Update(float dt) {

	for (int i = 0; i < ui_elements.Count(); i++)
		if (ui_elements.At(i) != nullptr) ui_elements[i]->Draw(atlas);

	return true;
}

// Called after all Updates
bool j1UI::PostUpdate()
{

	for (int i = ui_elements.Count() - 1; i >=0 ; i--) {
		if (ui_elements[i]->to_destroy) {
			delete(ui_elements[i]);
			ui_elements[i] = nullptr;
			if (!ui_elements.RemoveAt(i)) {
				LOG("Error removing ui_element");
				return false;
			}
		}
	}

	return true;
}

// Called before quitting
bool j1UI::CleanUp()
{
	LOG("Freeing GUI");
	//TODO unload tex

	for (uint i = 0; i < ui_elements.Count(); ++i)
	{
		if (ui_elements[i] != nullptr)
		{
			delete ui_elements[i];
			ui_elements[i] = nullptr;
			if (!ui_elements.RemoveAt(i)) {
				LOG("Error removing ui_element");
				return false;
			}
		}
	}

	ui_elements.Clear();

	return true;
}

// const getter for atlas
const SDL_Texture* j1UI::GetAtlas() const
{
	return atlas;
}

bool j1UI::DeleteUIElement(UIElement &element) {
	
	for (int i = 0; i < ui_elements.Count(); i++) {
		if (*ui_elements.At(i) == &element) {
			ui_elements[i]->to_destroy = true;
			return true;
		}
	}

	return false;
}

bool j1UI::DeleteAllUIElements() {
	bool ret = false;

	for (int i = 0; i < ui_elements.Count(); i++) {
		if (ui_elements.At(i) != nullptr) ui_elements[i]->to_destroy = true;
		ret = true;
	}

	return ret;
}

UIElement* j1UI::AddUIImage(int position_x, int position_y, SDL_Rect rect, j1Module* callback, UIElement* parent) {

	UIElement* tmp_img = new UIImage(position_x, position_y, IMAGE, rect, callback, parent);
	ui_elements.PushBack(tmp_img);
	return tmp_img;

	LOG("Error: Cant add the UIImage");
	return nullptr;
}

UIElement* j1UI::AddUIButton(int position_x, int position_y, SDL_Rect normal_rect, SDL_Rect focused_rect, SDL_Rect pressed_rect, j1Module* callback, UIElement* parent) {

	UIElement* tmpBtn = new UIButton(position_x, position_y, BUTTON, normal_rect,focused_rect,pressed_rect, callback, parent);
	ui_elements.PushBack(tmpBtn);
	return tmpBtn;

	LOG("Error: Cant add the UIButton");
	return nullptr;
}

UIElement* j1UI::AddUILabel(int position_x, int position_y, p2SString text, Color color, int size, UIElement* parent) {

	UIElement* tmp_lbl = new UILabel(position_x, position_y, LABEL, text, color, size, parent);
	ui_elements.PushBack(tmp_lbl);
	return tmp_lbl;
	
	LOG("Error: Cant add the UILabel");
	return nullptr;
}

UIElement * j1UI::AddUISlider(int position_x, int position_y, SDL_Rect slider_rect, j1Module * callback, UIElement * parent)
{
	UIElement* tmp_sldr = new UISlider(position_x, position_y, SLIDER, slider_rect, callback, parent);
	ui_elements.PushBack(tmp_sldr);
	return tmp_sldr;

	LOG("Error: Cant add the UIButton");
	return nullptr;
}

UIElement * j1UI::AddUICheckBox(int x, int y, SDL_Rect cb_tick_normal, SDL_Rect cb_no_tick_normal, SDL_Rect cb_tick_pressed, SDL_Rect cb_no_tick_pressed, SDL_Rect cb_tick_focus, SDL_Rect cb_no_tick_focus, j1Module * callback, UIElement * parent)
{
	UIElement* tmpCb = new UICheckBox(x, y, CHECK_BOX, cb_tick_normal, cb_no_tick_normal, cb_tick_pressed, cb_no_tick_pressed, cb_tick_focus, cb_no_tick_focus, callback, parent);
	ui_elements.PushBack(tmpCb);
	return tmpCb;

	LOG("Error: Cant add the UICheckBox");

	return nullptr;
}

UIElement* j1UI::GetElementUnderMouse(int x, int y)
{
	ui_elements.Flip();
	for (int i = 0; i < ui_elements.Count(); i++) {
		if (ui_elements[i] != nullptr && ui_elements[i]->interactable)
		{
			if ((x > ui_elements[i]->GetScreenPosition().x && x < ui_elements[i]->GetScreenPosition().x + ui_elements[i]->GetRect().w) && (y > ui_elements[i]->GetScreenPosition().y && y < ui_elements[i]->GetScreenPosition().y + ui_elements[i]->GetRect().h))
			{
				UIElement* tmp_element_to_return = ui_elements[i];
				ui_elements.Flip();
				return tmp_element_to_return;
			}
		}
	}
	ui_elements.Flip();
	return nullptr;
}

// class Gui ---------------------------------------------------

