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

	int width = 0, height = 0;
	App->font->CalcSize(this->text.GetString(), width, height, App->font->default);
	current_rect.w = width;
	current_rect.h = height;
}

void UILabel::SetTextFromNum(int value)
{

	std::string s = std::to_string(value);
 	std::string t("%%");

	s += t;

	text = s.c_str();
	//text.create(s.c_str());

	App->tex->UnLoad(texture);

	if (text_color == YELLOW)
		texture = App->font->Print(text.GetString(), { 254, 203,0,255 }, App->font->default,size);
	else if (text_color == GREY)
		texture = App->font->Print(text.GetString(), { 134, 136, 138,255 }, App->font->default,size);
	else if (text_color == BLACK)
		texture = App->font->Print(text.GetString(), { 0, 0, 0, 255 }, App->font->default,size);

	int width = 0, height = 0;
	App->font->CalcSize(this->text.GetString(), width, height, App->font->default);
	current_rect.w = width;
	current_rect.h = height;
}
