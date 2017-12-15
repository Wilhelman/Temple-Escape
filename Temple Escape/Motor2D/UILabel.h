#ifndef __UILABEL_H__
#define __UILABEL_H__

#include "UIElement.h"
#include "p2SString.h"
#include "j1Fonts.h"


class UILabel : public UIElement
{
private:
	j1Fonts normal_font;
	
	int size = 0;
	Color text_color = COLOR_NOT_DEF;
public:
	UILabel(int x, int y, UI_Type type, p2SString text, Color color, int size, UIElement* parent = nullptr);
	void SetText(p2SString text);
	void SetTextFromNum(int value, bool withPercetage = false); 

public:
	p2SString text;
};

#endif //__UILABEL_H__