#include "j1App.h"
#include "UILabel.h"
#include "p2Log.h"
#include "j1Textures.h"

UILabel::UILabel(int x, int y, UI_Type type, p2SString text, Color color, int size, UIElement* parent) : UIElement(x,y,type,parent)
{
	this->text = text;
	this->text_color = color;
	this->size = size;

	if (color == YELLOW)
		texture = App->font->Print(text.GetString(), { 254, 203, 0, 255 }, App->font->default, size);
	else if (color == GREY)
		texture = App->font->Print(text.GetString(), { 134, 136, 138, 255 }, App->font->default,size);
	else if (color == BLACK)
		texture = App->font->Print(text.GetString(), { 0, 0, 0, 255 }, App->font->default,size);
	else if (color == WHITE)
		texture = App->font->Print(text.GetString(), { 255, 255, 255, 255 }, App->font->default, size);

	int width = 0, height = 0;
	App->font->CalcSize(this->text.GetString(), width, height, App->font->default);
	current_rect.w = width;
	current_rect.h = height;
}

UILabel::UILabel(int x, int y, UI_Type type, p2SString text, Color color, int size, uint32 boxWidth, UIElement* parent) : UIElement(x, y, type, parent)
{
	this->text = text;
	this->text_color = color;
	this->size = size;

	if (color == YELLOW)
		texture = App->font->PrintTextBox(text.GetString(), { 254, 203, 0, 255 }, App->font->default, boxWidth, size);
	else if (color == GREY)
		texture = App->font->PrintTextBox(text.GetString(), { 134, 136, 138, 255 }, App->font->default, boxWidth, size);
	else if (color == BLACK)
		texture = App->font->PrintTextBox(text.GetString(), { 0, 0, 0, 255 }, App->font->default, boxWidth, size);
	else if (color == WHITE)
		texture = App->font->PrintTextBox(text.GetString(), { 255, 255, 255, 255 }, App->font->default, boxWidth, size);

	int width = 0, height = 0;
	App->font->CalcSize(this->text.GetString(), width, height, App->font->default);
	current_rect.w = width;
	current_rect.h = height;
}

void UILabel::SetText(p2SString text) 
{
	App->tex->UnLoad(texture);

	this->text= text;

	if (text_color == YELLOW)
		texture = App->font->Print(text.GetString(), { 254, 203,0,255 }, App->font->default, size);
	else if (text_color == GREY)
		texture = App->font->Print(text.GetString(), { 134, 136, 138,255 }, App->font->default, size);
	else if (text_color == BLACK)
		texture = App->font->Print(text.GetString(), { 0, 0, 0, 255 }, App->font->default, size);
	else if (text_color == WHITE)
		texture = App->font->Print(text.GetString(), { 255, 255, 255, 255 }, App->font->default, size);

	int width = 0, height = 0;
	App->font->CalcSize(this->text.GetString(), width, height, App->font->default);
	current_rect.w = width;
	current_rect.h = height;
}


void UILabel::SetTextFromNum(int value, bool withPercentage)
{
	std::string s = std::to_string(value);
	
	if (withPercentage) {
		std::string t("%%");
		s += t;
	}

	text = s.c_str();

	App->tex->UnLoad(texture);

	if (text_color == YELLOW)
		texture = App->font->Print(text.GetString(), { 254, 203,0,255 }, App->font->default,size);
	else if (text_color == GREY)
		texture = App->font->Print(text.GetString(), { 134, 136, 138,255 }, App->font->default,size);
	else if (text_color == BLACK)
		texture = App->font->Print(text.GetString(), { 0, 0, 0, 255 }, App->font->default,size);
	else if (text_color == WHITE)
		texture = App->font->Print(text.GetString(), { 255, 255, 255, 255 }, App->font->default, size);

	int width = 0, height = 0;
	App->font->CalcSize(this->text.GetString(), width, height, App->font->default);
	current_rect.w = width;
	current_rect.h = height;
}

void UILabel::SetHourFromMs(uint time_in_ms)
{

	float sec = (float)time_in_ms / 1000.0f;
	float min = sec / 60.0f;
	float hour = min / 60.0f;

	int sec_int = (int)sec;

	if (min < 1.0f)
		min = 0.0f;
	if (sec < 1.0f)
		sec = 0.0f;
	if (hour < 1.0f)
		hour = 0.0f;

	if (min > 0.0f)
		sec_int -= (int)min*60;

	p2SString tmp;
	tmp.create("%i : %i : %i", (int)hour, (int)min, sec_int);
	text.create(tmp.GetString());

	App->tex->UnLoad(texture);

	if (text_color == YELLOW)
		texture = App->font->Print(text.GetString(), { 254, 203,0,255 }, App->font->default, size);
	else if (text_color == GREY)
		texture = App->font->Print(text.GetString(), { 134, 136, 138,255 }, App->font->default, size);
	else if (text_color == BLACK)
		texture = App->font->Print(text.GetString(), { 0, 0, 0, 255 }, App->font->default, size);
	else if (text_color == WHITE)
		texture = App->font->Print(text.GetString(), { 255, 255, 255, 255 }, App->font->default, size);

	int width = 0, height = 0;
	App->font->CalcSize(this->text.GetString(), width, height, App->font->default);
	current_rect.w = width;
	current_rect.h = height;
}

p2SString UILabel::GetTextFromNum(int num) const
{
	std::string s = std::to_string(num);
	p2SString to_ret;
	to_ret.create(s.c_str());

	return to_ret;
}