#include "Styles.h"
#include "Draw.h"

int DefaultStyle::ControlPanel(int x, int y, int w, int h, const char* name)
{
	g_Draw.OutlineRect(x, y, w, h, Color(50, 50, 55));

	int txtMargin = 0;

	int textW = 0, textH = 15;
	if (name)
		g_Interfaces.Surface->GetTextSize(gFonts.calibri_light_small, ToWchar(name).c_str(), textW, textH);
	g_Draw.DrawRect(x + 1, y + 1, w - 2, textH + txtMargin, Color(25));
	if (name)
		g_Draw.DrawString(x + ((w / 2) - (textW / 2)), y, Color(150), name, gFonts.calibri_light_small);

	g_Draw.DrawLine(x, y + textH + txtMargin, x + w, y + textH + txtMargin, Color(50, 50, 55));
	x += 1, y += textH + txtMargin + 1, w -= 2, h -= textH + txtMargin + 2;

	g_Draw.DrawRect(x, y, w, h, Color(30, 30, 33));

	return textH + txtMargin + 1;
}

int DefaultStyle::DialogButton(int x, int y, int w, const char* text, bool mouseOver)
{
	g_Draw.DrawRect(x, y, w, 15, Color(42, 42, 48));
	g_Draw.OutlineRect(x, y, w, 15, Color(60));

	g_Draw.DrawString(x + 3, y, mouseOver ? Color(90, 150, 225) : Color(125, 125, 130), text, gFonts.verdana_bold);

	return 15;
}

void DefaultStyle::Dialog(int x, int y, int w, int h)
{
	g_Draw.OutlineRect(x, y, w, h, Color(58, 58, 70));
	g_Draw.DrawRect(x + 1, y + 1, w - 2, h - 2, Color(36, 36, 40));
}

#define TOPBAR 25
int DefaultStyle::TopBar(int x, int y, int w, const char* title)
{
	// Dark topbar
	g_Draw.DrawRect(x, y, w, TOPBAR, Color(32, 34, 37));

	if (title)
		g_Draw.DrawString(x + 10, y, Color(120), title, gFonts.segoe);

	return TOPBAR;
}