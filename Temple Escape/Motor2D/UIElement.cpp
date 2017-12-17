#include "j1App.h"
#include "UIElement.h"
#include "j1Render.h"
#include "j1UI.h"
#include "j1Input.h"
#include "j1Textures.h"


UIElement::UIElement(int x, int y, UI_Type type, UIElement* parent) : local_position(x, y), type(type), parent(parent)
{
	current_state = STATE_NORMAL;
	this->interactable = true;

	if (parent == nullptr) 
	{
		screen_position.x = App->render->ScreenToWorld(x, y).x;
		screen_position.y = App->render->ScreenToWorld(x, y).y;
	}
	else
	{
		screen_position.x = parent->screen_position.x + local_position.x;
		screen_position.y = parent->screen_position.y + local_position.y;
	}
}

UIElement::~UIElement()
{
	App->tex->UnLoad(texture);
	texture = nullptr;
	callback = nullptr;
	parent = nullptr;
}

void UIElement::Update()
{
	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
		debug_draw = !debug_draw;

	int mouse_x = 0, mouse_y = 0;
	App->input->GetWorldMousePosition(mouse_x, mouse_y);

	UIElement* element_to_trigger = nullptr;
	element_to_trigger = App->ui->GetElementUnderMouse(mouse_x, mouse_y);

	if (element_to_trigger != nullptr && this == element_to_trigger)
	{
		if (current_state != STATE_MOUSE_ENTER && current_state != STATE_LEFT_MOUSE_PRESSED)
		{
			current_state = UI_State::STATE_MOUSE_ENTER;
			this->callback->OnUITrigger(this, current_state);
			last_state = current_state;
		}

		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT && current_state == STATE_MOUSE_ENTER)
		{
			current_state = STATE_LEFT_MOUSE_PRESSED;
			this->callback->OnUITrigger(this, current_state);
			last_state = current_state;
		}
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP && current_state == STATE_LEFT_MOUSE_PRESSED)
		{
			current_state = STATE_LEFT_MOUSE_RELEASED;
			this->callback->OnUITrigger(this, current_state);
			last_state = current_state;
		}

		if (current_state == STATE_LEFT_MOUSE_PRESSED)
		{
			if (this->draggable)
			{
				App->input->GetMouseMotion(mouse_motion_x, mouse_motion_y);

				if (parent != nullptr && parent->type == UI_Type::SLIDER)
				{
					if (mouse_x != tmp_mouse_x)
					{
						int next_x_right = screen_position.x + current_rect.w + mouse_motion_x;
						int next_x_left = screen_position.x + mouse_motion_x;

						if (next_x_right <= parent->screen_position.x + parent->current_rect.w
							&& next_x_left >= parent->screen_position.x)
						{
							screen_position.x += mouse_motion_x;
							local_position.x += mouse_motion_x;
						}
						else
						{
							if (next_x_right > parent->screen_position.x + parent->current_rect.w)
							{
								screen_position.x = parent->current_rect.w - current_rect.w;
								local_position.x = parent->current_rect.w - current_rect.w;
							}
							else
							{
								screen_position.x = parent->screen_position.x;
								local_position.x = 0;
							}
						}
						tmp_mouse_x = mouse_x;
						tmp_mouse_y = mouse_y;
					}
				}
				else if (mouse_x != tmp_mouse_x || mouse_y != tmp_mouse_y)
				{
					screen_position.x += mouse_motion_x;
					screen_position.y += mouse_motion_y;
					local_position.x += mouse_motion_x;
					local_position.y += mouse_motion_y;
					tmp_mouse_x = mouse_x;
					tmp_mouse_y = mouse_y;
				}
			}
		}
	}
	else
	{
		if (current_state == UI_State::STATE_MOUSE_ENTER || current_state == STATE_LEFT_MOUSE_PRESSED)
		{
			current_state = UI_State::STATE_MOUSE_LEAVE;
			this->callback->OnUITrigger(this, current_state);
			last_state = current_state;
		}
		else if (current_state == STATE_MOUSE_LEAVE) 
		{
			current_state = STATE_NORMAL;
			last_state = current_state;
		}
	}

	
		if (parent == nullptr)
			screen_position = App->render->ScreenToWorld(local_position.x, local_position.y);
		else
		{
			screen_position.x = parent->screen_position.x + local_position.x;
			screen_position.y = parent->screen_position.y + local_position.y;
		}
	
}

void UIElement::Draw(SDL_Texture* sprites)
{
	if (current_rect.w > 0 && current_rect.h > 0 && !invisible)
	{
		switch (this->type)
		{
		case IMAGE:
		case BUTTON:
		case SLIDER:
		case CHECK_BOX:
			App->render->Blit(sprites, screen_position.x, screen_position.y, &current_rect);
			break;
		case LABEL:
			App->render->Blit(texture, screen_position.x, screen_position.y, &current_rect);
			break;
		default:
			break;
		}

		if (debug_draw) {
			SDL_Rect tmp_rect{ screen_position.x,screen_position.y,current_rect.w,current_rect.h };
			App->render->DrawQuad(tmp_rect, 255, 0, 0, 255, false);
		}
	}
}

iPoint UIElement::GetLocalPosition() const
{
	return this->local_position;
}

iPoint UIElement::GetScreenPosition() const
{
	return this->screen_position;
}

void UIElement::SetParent(UIElement * parent)
{
	this->parent = parent;
}

UIElement * UIElement::GetParent() const
{
	return this->parent;
}

SDL_Rect UIElement::GetRect() const
{
	return current_rect;
}

void UIElement::SetLocalPosition(int x, int y) {
	this->local_position = iPoint(x, y);
}

void UIElement::SetScreenPosition(int x, int y) {
	this->screen_position = iPoint(x, y);
}