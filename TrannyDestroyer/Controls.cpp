#include "Controls.h"
#include "Draw.h"
#include "Menu.h"
#include "Styles.h"
#include "RGBA images.h"
#include <string>

static TextureHolder arrow_t, arrowside_t, arrowdown_t;
static TextureHolder colorpicker, brightness, picker_t;

#define LTEXT Color(220)
#define BTEXT Color(150)
#define HTEXT Color(90, 160, 225)
#define CTEXT Color(180)
#define BACKGR Color(65, 65, 72)

void InitTextures()
{

	// Hue and saturation gradient
	byte* coloures = new byte[128 * 128 * 4];
	int i = 0;
	for (int h = 0; h < 128; h++)
	{
		for (int w = 0; w < 128; w++)
		{
			Color hsv = hsv2rgb(float(w) / 128, float(128 - h) / 128.f, 1);
			coloures[i] = hsv[0], coloures[i + 1] = hsv[1], coloures[i + 2] = hsv[2], coloures[i + 3] = hsv[3];
			i += 4;
		}
	}
	i = 0;

	// Value gradient
	byte* value = new byte[32 * 128 * 4];
	for (int h = 0; h < 128; h++)
	{
		for (int w = 0; w < 32; w++)
		{
			Color clr(((128 - h) / 128.f) * 255.f, w > 15 ? 0 : 255);
			value[i] = clr[0], value[i + 1] = clr[1], value[i + 2] = clr[2], value[i + 3] = clr[3];
			i += 4;
		}
	}

	colorpicker = TextureHolder(coloures, 128, 128);
	brightness = TextureHolder(value, 32, 128);
	picker_t = TextureHolder(picker, 8, 8);
}

// ===== BaseControl =====

void BaseControl::RunControl(int Index)
{
	if (type == e_control::null)
		return;

	bool mouse = g_Menu.mouseOver(x, y, GetWidth(), GetHeight());
	if (g_Menu.GetFocus() == Index && !(flags & noinput) && mouse)
		HandleInput();
	if (IsVisible())
		Draw(mouse);
}

// ===== TabGroup =====

int TabGroup::Draw(bool mouseOver)
{
	h = GetHeight();

	int ty = y;
	for (size_t i = 0; i < tabs.size(); i++)
	{
		//if (tab->visible != nullptr && !*tab->visible)
			//continue;

		tabs[i]->SetWidth(w);
		tabs[i]->SetPos(x, ty);
		tabs[i]->enabled = tabs[i] == active;

		tabs[i]->HandleInput();
		tabs[i]->Draw(g_Menu.mouseOver(x, ty, w, GetTabHeight));

		ty += GetTabHeight + spacing;
	}

	return h;
}

void TabGroup::HandleInput()
{
	h = GetHeight();
	if (!g_Menu.mouseOver(x, y, w, h))
		return;

	int ty = y;
	for (size_t i = 0; i < tabs.size(); i++)
	{
		if (g_Menu.mouseOver(x, ty, w, GetTabHeight) && g_Menu.mb == e_mb::lclick)
		{
			active = tabs[i];
			break;
		}

		ty += GetTabHeight + spacing;
	}
}

// ===== Tab =====

int Tab::Draw(bool mouseOver)
{
	Color clr = enabled ? LTEXT : BTEXT;

	if (mouseOver)
		g_Draw.DrawRect(x, y, w, h, Color(18));
	g_Draw.DrawString(x + 20, y + 2, clr, name, gFonts.calibri_light);

	return h;
}

// ===== Groupbox =====

int Groupbox::Draw(bool mouseOver)
{
	// Initializing our height variable
	GetHeight();

	int strw, strh;
	g_Interfaces.Surface->GetTextSize(gFonts.verdana_bold, ToWchar(name).c_str(), strw, strh);

	y += (strh / 2);

	g_Draw.DrawLine(x, y, x, y + h, Color(80));
	g_Draw.DrawLine(x + w, y, x + w, y + h, Color(80));
	g_Draw.DrawLine(x, y + h, x + w, y + h, Color(80));

	g_Draw.DrawString(x + (w / 2) - (strw / 2), y - (strh / 2), Color(110), name, gFonts.verdana_bold);
	g_Draw.DrawLine(x, y, x + (w / 2) - (strw / 2) - 5, y, Color(80));
	g_Draw.DrawLine(x + (w / 2) + (strw / 2) + 5, y, x + w, y, Color(80));

	int cx = x + SPACING, cy = y + SPACING;
	for (size_t i = 0; i < children.size(); i++)
	{
		if (!children[i]->IsVisible())
			continue;

		children[i]->SetPos(cx, cy);
		children[i]->SetWidth(w - (SPACING * 2));

		bool over = g_Menu.mouseOver(cx, cy, children[i]->GetWidth(), children[i]->GetHeight());
		bool getInput = !(children[i]->flags & noinput) && over && !g_Menu.IsDialogOpen();

		children[i]->Draw(getInput);
		cy += children[i]->GetHeight() + SPACING;
	}

	y -= (strh / 2);

	return h;
}

void Groupbox::HandleInput()
{
	int strw, strh;
	g_Interfaces.Surface->GetTextSize(gFonts.verdana_bold, ToWchar(name).c_str(), strw, strh);

	int cx = x + SPACING, cy = y + SPACING + (strh / 2);
	for (size_t i = 0; i < children.size(); i++)
	{
		if (!children[i]->IsVisible())
			continue;

		children[i]->SetPos(cx, cy);
		children[i]->SetWidth(w - (SPACING * 2));

		bool over = g_Menu.mouseOver(cx, cy, children[i]->GetWidth(), children[i]->GetHeight());
		bool getInput = !(children[i]->flags & noinput) && over && !g_Menu.IsDialogOpen();
		if (getInput)
			children[i]->HandleInput();

		cy += children[i]->GetHeight() + SPACING;
	}
}

int Groupbox::GetHeight()
{
	if (!bAutoHeight)
		return h;

	h = SPACING;
	for (size_t i = 0; i < children.size(); i++)
	{
		if (!children[i]->IsVisible())
			continue;
		h += children[i]->GetHeight() + SPACING;
	}

	int strw, strh;
	g_Interfaces.Surface->GetTextSize(gFonts.verdana_bold, ToWchar(name).c_str(), strw, strh);

	return h + (strh / 2);
}

// ===== Checkbox =====

int Checkbox::Draw(bool mouseOver)
{
	if (value)
		g_Draw.DrawRect(x + 2, y + 2, h - 4, h - 4, Color(35, 135, 230));

	g_Draw.OutlineRect(x, y, h, h, Color(100));
	g_Draw.DrawString(x + 18, y - 1, mouseOver ? HTEXT : CTEXT, name, gFonts.verdana_bold);

	return h;
}

void Checkbox::HandleInput()
{
	if (g_Menu.mb == e_mb::lclick || g_Menu.mb == e_mb::rclick)
		value = !value;
}

bool Checkbox::QuickReturn(bool Value, int X, int Y, int W)
{
	int Width = W == -1 ? w : W;
	bool hovered = g_Menu.mouseOver(X, Y, Width, h);
	value = Value, x = X, y = Y, w = Width;
	Draw(hovered);
	if (hovered)
		HandleInput();

	return value;
}

// ===== DrawPanel =====

int DrawPanel::Draw(bool mouseOver)
{
	// Draw a control panel, then draw our stuff within it
	int topbar = g_Menu.style->ControlPanel(x, y, w, h, name);
	draw(x, y + topbar, w, h - topbar);

	return h;
}

// ===== Slider =====

int Slider::Draw(bool mouseOver)
{
	int nw = w - 30;
	Color clr = mouseOver ? HTEXT : CTEXT;
	g_Draw.DrawString(x, y, clr, name, gFonts.verdana_bold);
	g_Draw.DrawRect(x, y + 17, nw, 4, BACKGR);

	int percent = nw * (value - min) / (max - min);
	if (mouseOver)
	{
		g_Draw.DrawRect(x, y + 17, percent, 4, HTEXT);
		arrow_t.Draw(x + percent - 3, y + 17, CTEXT);
	}
	else
		g_Draw.DrawRect(x, y + 17, percent, 4, Color(85, 85, 95));
	g_Draw.DrawString(x + nw + 3, y + 9, Color(125, 125, 130), to_string(value), gFonts.verdana_bold);

	return h;
}

void Slider::HandleInput()
{
	static bool bDrag = false;
	if (g_Menu.mb == e_mb::lclick)
		bDrag = true;
	else if (g_Menu.mb == e_mb::null)
		bDrag = false;

	if (g_Menu.mw != e_mw::null)
		value += (g_Menu.mw == e_mw::down) ? step : -step;
	else if (bDrag && g_Menu.mb == e_mb::ldown)
	{
		float percent = float(g_Menu.mouse.x - x) / (float(w) - 30.f);
		if (percent > 1.f)
			percent = 1.f;
		else if (percent < 0.f)
			percent = 0.f;

		value = (percent * abs(min - max)) - min;
	}

	if (value > max)
		value = min;
	else if (value < min)
		value = max;
}

// ===== Listbox =====
#define TEXTH 15
int Listbox::Draw(bool mouseOver)
{
	Color clr = mouseOver ? HTEXT : CTEXT;

	g_Draw.DrawString(x, y, clr, name, gFonts.verdana_bold);

	g_Menu.style->DialogButton(x, y + TEXTH, w, list[value].c_str(), mouseOver);

	arrowdown_t.Draw(x + w - 10, y + TEXTH + 6, Color(125, 125, 140));
	return h;
}

void DrawListbox(void* data, size_t Index);
static Dialog listDlg(DrawListbox);

void Listbox::HandleInput()
{
	if (value > list.size())
		value = list.size() - 1;

	if (g_Menu.mb != e_mb::lclick)
		return;

	listDlg.data = this;
	g_Menu.OpenDialog(listDlg);
}

void DrawListbox(void* data, size_t Index)
{
	BaseControl* control = (BaseControl*)data;
	if (control == nullptr || control->type != e_control::listbox)
		return g_Menu.CloseDialog(Index);

	Listbox* listbox = (Listbox*)control;
	int x = listbox->x, y = listbox->y + listbox->GetHeight(), w = listbox->GetWidth(), h = listbox->list.size() * 16 + 20;

	if (g_Menu.mb == e_mb::lclick && !g_Menu.mouseOver(x, y - listbox->GetHeight(), w, h + listbox->GetHeight()))
		return g_Menu.CloseDialog(Index);

	g_Menu.style->Dialog(x, y, w, h);

	x += 10, y += 8, w -= 20, h -= 20;
	for (size_t i = 0; i < listbox->list.size(); i++)
	{
		if (g_Menu.mouseOver(x, y, w, 15))
		{
			if (g_Menu.mb == e_mb::lclick)
			{
				listbox->value = i;
				return g_Menu.CloseDialog(Index);
			}

			g_Draw.DrawRect(x, y, w, 16, Color(44, 44, 55));
		}
		g_Draw.DrawString(x, y + 1, CTEXT, listbox->list[i], gFonts.verdana_bold);
		y += 16;
	}
}

// ===== Color =====

int ColorPicker::Draw(bool mouseOver)
{
	g_Draw.DrawString(x, y, mouseOver ? HTEXT : CTEXT, name, gFonts.verdana_bold);
	g_Draw.DrawRect(x, y + 17, w, 5, color);

	if (!mouseOver)
		g_Draw.DrawRect(x, y + 17, w, 5, Color(0, 150));

	return h;
}

void DrawColorBox(void* data, size_t Index);
static Dialog colorDlg(DrawColorBox);

void ColorPicker::HandleInput()
{
	if (g_Menu.mb != e_mb::lclick)
		return;

	colorDlg.x = x, colorDlg.y = y;
	colorDlg.data = this;
	g_Menu.OpenDialog(colorDlg);
}

void DrawColorBox(void* data, size_t Index)
{
	BaseControl* control = (BaseControl*)data;
	if (control == nullptr || control->type != e_control::colorpicker)
		return g_Menu.CloseDialog(Index);

	if (g_Menu.mb == e_mb::lclick && !g_Menu.mouseOver(colorDlg.x, colorDlg.y, 175, 185))
		return g_Menu.CloseDialog(Index);

	g_Menu.style->Dialog(colorDlg.x, colorDlg.y, 175, 185);
	int X = colorDlg.x + 2, Y = colorDlg.y + 2, W = 171, H = 181;

	static int ccursorx = 50, ccursory = 50;
	static int vcursory = 0;

	static bool mColorbox = false, mValue = false;
	if (g_Menu.mb == e_mb::lclick && g_Menu.mouseOver(X + 5, Y + 30, colorpicker.GetWidth(), colorpicker.GetHeight()))
		mColorbox = true;
	else if (g_Menu.mb != e_mb::ldown)
		mColorbox = false;

	if (g_Menu.mb == e_mb::lclick && g_Menu.mouseOver(X + colorpicker.GetWidth() + 10, Y + 30, 15, brightness.GetHeight()))
		mValue = true;
	else if (g_Menu.mb != e_mb::ldown)
		mValue = false;

	if (mColorbox)
	{
		ccursorx = g_Menu.mouse.x - X - 5, ccursory = g_Menu.mouse.y - Y - 30;

		if (ccursorx < 0)
			ccursorx = 0;
		else if (ccursorx > colorpicker.GetWidth())
			ccursorx = colorpicker.GetWidth();

		if (ccursory < 0)
			ccursory = 0;
		else if (ccursory > colorpicker.GetHeight() - 1)
			ccursory = colorpicker.GetHeight() - 1;
	}
	else if (mValue)
	{
		vcursory = g_Menu.mouse.y - Y - 30;

		if (vcursory < 0)
			vcursory = 0;
		else if (vcursory > brightness.GetHeight() - 1)
			vcursory = brightness.GetHeight() - 1;
	}

	ColorPicker* color = (ColorPicker*)control;

	static Checkbox checkbox("Default");
	color->bDef = checkbox.QuickReturn(color->bDef, X + 5, Y + 30 + colorpicker.GetHeight() + 5);

	if (!color->bDef)
		color->color = hsv2rgb(float(ccursorx) / colorpicker.GetWidth(), 1.f - (float(ccursory) / colorpicker.GetHeight()), 1.f - (float(vcursory) / brightness.GetHeight()));
	else
		color->color = color->cDef;

	g_Draw.OutlineRect(X + 4, Y + 29, colorpicker.GetWidth() + 2, colorpicker.GetHeight() + 2, Color(58, 58, 70));
	colorpicker.Draw(X + 5, Y + 30, Color(255 - (float(vcursory) / brightness.GetHeight()) * 255));

	g_Draw.OutlineRect(X + colorpicker.GetWidth() + 9, Y + 29, 16 + 2, brightness.GetHeight() + 2, Color(58, 58, 70));
	brightness.Draw(X + colorpicker.GetWidth() + 10, Y + 30);

	g_Draw.OutlineRect(X + 4, Y + 4, colorpicker.GetWidth() + 23, 22, Color(58, 58, 70));
	g_Draw.DrawRect(X + 5, Y + 5, colorpicker.GetWidth() + 21, 20, color->color);

	picker_t.Draw(X + 5 + ccursorx - 4, Y + 30 + ccursory - 4);
	arrowside_t.Draw(X + colorpicker.GetWidth() + 30, Y + 30 + vcursory);
}

// ===== KeyBind =====

Checkbox KeyBind::check = Checkbox("");
int KeyBind::Draw(bool mouseOver)
{
	int iW = check.GetHeight(), iX = x + iW + 5;
	if (mouseOver)
		mouseOver = !g_Menu.mouseOver(x, y + TEXTH + (TEXTH / 2) - (iW / 2), iW, iW);
	Color clr = mouseOver ? HTEXT : CTEXT;

	bEnabled = check.QuickReturn(bEnabled, x, y + TEXTH + (TEXTH / 2) - (iW / 2), iW);

	g_Draw.DrawString(x, y, clr, name, gFonts.verdana_bold);

	g_Draw.DrawRect(iX, y + TEXTH, w - iW - 5, TEXTH, Color(42, 42, 48));
	g_Draw.OutlineRect(iX, y + TEXTH, w - iW - 5, TEXTH, Color(60));

	string keyStr = bEnabled ? mode == e_kbmode::always ? "Always on" : gKey.KeyToString(key) : "Off";
	g_Draw.DrawString(iX + 3, y + TEXTH, mouseOver ? clr : Color(125, 125, 130), keyStr, gFonts.verdana_bold);

	g_Draw.DrawRect(iX + w - iW - 15, y + TEXTH + 5, 6, 1, Color(125, 125, 140));
	g_Draw.DrawRect(iX + w - iW - 15, y + TEXTH + 7, 6, 1, Color(125, 125, 140));
	g_Draw.DrawRect(iX + w - iW - 15, y + TEXTH + 9, 6, 1, Color(125, 125, 140));

	return h;
}

void DrawKeyBindBox(void* data, size_t Index);
static Dialog keyDlg(DrawKeyBindBox);

void KeyBind::HandleInput()
{
	if (g_Menu.mb != e_mb::lclick)
		return;

	int iW = check.GetHeight();
	if (g_Menu.mouseOver(x, y + TEXTH + (TEXTH / 2) - (iW / 2), iW, iW))
		return;

	keyDlg.x = x, keyDlg.y = y;
	keyDlg.h = 100;

	keyDlg.data = this;
	g_Menu.OpenDialog(keyDlg);
}
bool KeyBind::UpdateKeys()
{
	if (!g_Menu.last_key || !gKey.AcceptedKey(g_Menu.last_key))
		return false;

	if (g_Menu.last_key == VK_ESCAPE)
		return true;

	key = g_Menu.last_key;
	return true;
}

bool KeyBind::KeyDown()
{
	if (!bEnabled)
		return false;

	switch (mode)
	{
	case e_kbmode::always:
		return true;
	case e_kbmode::hold:
		return g_Menu.keys[key];
	case e_kbmode::toggle:
		if (!g_Menu.keys[key])
			keyHeld = false;
		if (!keyHeld && g_Menu.keys[key])
			toggled = !toggled, keyHeld = true;

		return toggled;
	}
	return false;
}

void DrawKeyBindBox(void* data, size_t Index)
{
	BaseControl* control = (BaseControl*)data;
	if (control == nullptr || control->type != e_control::keybind)
		return g_Menu.CloseDialog(Index);

	static bool bWait = false;
	if (g_Menu.mb == e_mb::lclick && !bWait && !g_Menu.mouseOver(keyDlg.x, keyDlg.y, keyDlg.w, keyDlg.h))
		return g_Menu.CloseDialog(Index);

	KeyBind* kb = (KeyBind*)control;

	g_Menu.style->Dialog(keyDlg.x, keyDlg.y, keyDlg.w, keyDlg.h);
	int x = keyDlg.x + 2, y = keyDlg.y + 2, w = keyDlg.w - 4, h = keyDlg.h - 4;

	g_Draw.DrawString(x + 5, y + 5, Color(100), kb->name, gFonts.verdana_bold);
	y += 21, h -= 21;

	bool hover_setkey = g_Menu.mouseOver(x + 6, y + 6, w - 12, 18);
	if (!bWait && hover_setkey && g_Menu.mb == e_mb::lclick)
		bWait = true;

	if (bWait)
		bWait = !kb->UpdateKeys();

	g_Draw.OutlineRect(x + 6, y + 6, w - 12, 18, Color(80));
	g_Draw.DrawRect(x + 7, y + 7, w - 14, 16, Color(hover_setkey ? 65 : 40));
	g_Draw.DrawString(x + 9, y + 8, Color(200), bWait ? "Select a key (or Escape)" : gKey.KeyToString(kb->key), gFonts.verdana_bold);

	static Listbox list("Setting", { "Always on", "Hold key", "Toggle with key" });
	list.SetPos(x + 6, y + 30), list.SetWidth(w - 12);

	static bool list_open = false;
	if (g_Menu.GetFocus() == Index && list_open)
		kb->mode = (e_kbmode)list.value, list_open = false;

	list_open = g_Menu.GetFocus() != Index;

	list.value = (int)kb->mode;
	list.RunControl(Index);
}