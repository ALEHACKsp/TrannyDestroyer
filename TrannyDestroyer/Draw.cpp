#include "Draw.h"
#include "Engine.h"
#include "Base.h"
#include <string>

//===================================================================================
CDraw g_Draw;
CFonts gFonts;
CTextures gTex;
CScreenSize gScreenSize;

#define ESP_HEIGHT 14
//===================================================================================
void CDraw::Init() // Create font IDs here
{
	if (g_Interfaces.Surface == NULL)
		return;

	gFonts.Initialize();
	gTex.Initialize();

	Reload();
}
void CDraw::Reload() // Create (or reload) your font styles
{
	if (g_Interfaces.Surface == NULL)
		return;

	g_Interfaces.Engine->GetScreenSize(gScreenSize.width, gScreenSize.height);

	gFonts.Reload();
	gTex.Reload();
}
//===================================================================================
void CFonts::Initialize()
{
	arial = g_Interfaces.Surface->CreateFont();
	segoe = g_Interfaces.Surface->CreateFont();
	verdana_bold = g_Interfaces.Surface->CreateFont();
	verdana = g_Interfaces.Surface->CreateFont();
	calibri = g_Interfaces.Surface->CreateFont();
	calibri_light = g_Interfaces.Surface->CreateFont();
	calibri_light_small = g_Interfaces.Surface->CreateFont();
	cambria = g_Interfaces.Surface->CreateFont();
	esp = g_Interfaces.Surface->CreateFont();
}
void CFonts::Reload()
{
	g_Interfaces.Surface->SetFontGlyphSet(gFonts.arial, "Arial", ESP_HEIGHT, 500, 0, 0, FONTFLAG_OUTLINE);
	g_Interfaces.Surface->SetFontGlyphSet(gFonts.segoe, "Segoe UI", 24, 250, 0, 0, FONTFLAG_ANTIALIAS);
	g_Interfaces.Surface->SetFontGlyphSet(gFonts.verdana_bold, "Verdana", 14, 800, 0, 0, FONTFLAG_ANTIALIAS);
	g_Interfaces.Surface->SetFontGlyphSet(gFonts.verdana, "Verdana", 14, 0, 0, 0, FONTFLAG_ANTIALIAS);
	g_Interfaces.Surface->SetFontGlyphSet(gFonts.calibri, "Calibri", 24, 500, 0, 0, FONTFLAG_ANTIALIAS);
	g_Interfaces.Surface->SetFontGlyphSet(gFonts.calibri_light, "Calibri Light", 24, 250, 0, 0, FONTFLAG_ANTIALIAS);
	g_Interfaces.Surface->SetFontGlyphSet(gFonts.calibri_light_small, "Calibri Light", 18, 250, 0, 0, FONTFLAG_ANTIALIAS);
	g_Interfaces.Surface->SetFontGlyphSet(gFonts.cambria, "Cambria", 48, 1000, 0, 0, FONTFLAG_ANTIALIAS);
	g_Interfaces.Surface->SetFontGlyphSet(gFonts.esp, "Tahoma", 12, 800, 0, 0, FONTFLAG_OUTLINE);
}
//===================================================================================
void CTextures::Initialize()
{
	team_blue = g_Interfaces.Surface->CreateNewTextureID();
	team_red = g_Interfaces.Surface->CreateNewTextureID();
	dark_gray = g_Interfaces.Surface->CreateNewTextureID();
}
void CTextures::Reload()
{
	static const byte dark_gray_t[4] = { 50, 50, 50, 255 };
	g_Interfaces.Surface->DrawSetTextureRGBA(dark_gray, dark_gray_t, 1, 1);
	static const byte team_blue_t[4] = { 0, 128, 255, 255 };
	g_Interfaces.Surface->DrawSetTextureRGBA(team_blue, team_blue_t, 1, 1);
	static const byte team_red_t[4] = { 186, 52, 53, 255 };
	g_Interfaces.Surface->DrawSetTextureRGBA(team_red, team_red_t, 1, 1);
}
//===================================================================================
void CDraw::DrawString(int x, int y, Color color, string text, HFont font)
{
	DrawString(x, y, color, text.c_str(), font);
}
void CDraw::DrawString(int x, int y, Color color, wstring text, HFont font)
{
	DrawString(x, y, color, text.c_str(), font);
}
void CDraw::DrawString(int x, int y, Color color, const char *pszText, HFont font)
{
	if (pszText == NULL)
		return;

	va_list va_alist;
	char szBuffer[1024] = { '\0' };
	wchar_t szString[1024] = { '\0' };

	va_start(va_alist, pszText);
	vsprintf_s(szBuffer, pszText, va_alist);
	va_end(va_alist);

	wsprintfW(szString, L"%S", szBuffer);

	DrawString(x, y, color, szString, font);
}
void CDraw::DrawString(int x, int y, Color color, const wchar_t* wcText, HFont font)
{
	g_Interfaces.Surface->DrawSetTextPos(x, y);
	g_Interfaces.Surface->DrawSetTextFont(font);
	g_Interfaces.Surface->DrawSetTextColor(color[0], color[1], color[2], color[3]);
	g_Interfaces.Surface->DrawPrintText(wcText, wcslen(wcText));
}
//===================================================================================
byte CDraw::GetESPHeight()
{
	return ESP_HEIGHT;
}
//===================================================================================
void CDraw::DrawLine(int x, int y, int x1, int y1, Color color)
{
	g_Interfaces.Surface->DrawSetColor(color[0], color[1], color[2], color[3]);
	g_Interfaces.Surface->DrawLine(x, y, x1, y1);
}
//===================================================================================
void CDraw::DrawRect(int x, int y, int w, int h, Color color)
{
	g_Interfaces.Surface->DrawSetColor(color[0], color[1], color[2], color[3]);
	g_Interfaces.Surface->DrawFilledRect(x, y, x + w, y + h);
}
//===================================================================================
void CDraw::OutlineRect(int x, int y, int w, int h, Color color)
{
	g_Interfaces.Surface->DrawSetColor(color[0], color[1], color[2], color[3]);
	g_Interfaces.Surface->DrawOutlinedRect(x, y, x + w, y + h);
}
//===================================================================================
void CDraw::DrawBox(Vector vOrigin, Color color, int box_width, int radius)
{
	Vector vScreen;

	if (!WorldToScreen(vOrigin, vScreen))
		return;

	int radius2 = radius << 1;

	OutlineRect(vScreen.x - radius + box_width, vScreen.y - radius + box_width, radius2 - box_width, radius2 - box_width, 0x000000FF);
	OutlineRect(vScreen.x - radius - 1, vScreen.y - radius - 1, radius2 + (box_width + 2), radius2 + (box_width + 2), 0x000000FF);
	DrawRect(vScreen.x - radius + box_width, vScreen.y - radius, radius2 - box_width, box_width, color);
	DrawRect(vScreen.x - radius, vScreen.y + radius, radius2, box_width, color);
	DrawRect(vScreen.x - radius, vScreen.y - radius, box_width, radius2, color);
	DrawRect(vScreen.x + radius, vScreen.y - radius, box_width, radius2 + box_width, color);
}
//===================================================================================
void CDraw::DrawPolygon(int count, Vertex_t* verts, int texture)
{
	g_Interfaces.Surface->DrawSetColor(255, 255, 255, 255); // keep this full color and opacity use the RGBA @top to set values.
	g_Interfaces.Surface->DrawSetTexture(texture); // bind texture
	g_Interfaces.Surface->DrawTexturedPolygon(count, verts);
}
//===================================================================================
bool CDraw::WorldToScreen(Vector &vOrigin, Vector &vScreen)
{
	const matrix3x4& worldToScreen = g_Interfaces.Engine->WorldToScreenMatrix(); //Grab the world to screen matrix from CEngineClient::WorldToScreenMatrix

	float w = worldToScreen[3][0] * vOrigin[0] + worldToScreen[3][1] * vOrigin[1] + worldToScreen[3][2] * vOrigin[2] + worldToScreen[3][3]; //Calculate the angle in compareson to the player's camera.
	vScreen.z = 0; //Screen doesn't have a 3rd dimension.

	if (w > 0.001) //If the object is within view.
	{
		float fl1DBw = 1 / w; //Divide 1 by the angle.
		vScreen.x = (gScreenSize.width / 2) + (0.5 * ((worldToScreen[0][0] * vOrigin[0] + worldToScreen[0][1] * vOrigin[1] + worldToScreen[0][2] * vOrigin[2] + worldToScreen[0][3]) * fl1DBw) * gScreenSize.width + 0.5); //Get the X dimension and push it in to the Vector.
		vScreen.y = (gScreenSize.height / 2) - (0.5 * ((worldToScreen[1][0] * vOrigin[0] + worldToScreen[1][1] * vOrigin[1] + worldToScreen[1][2] * vOrigin[2] + worldToScreen[1][3]) * fl1DBw) * gScreenSize.height + 0.5); //Get the Y dimension and push it in to the Vector.
		return true;
	}

	return false;
}
//===================================================================================
inline void ClampFl(float &fl)
{
	if (fl > 1)
		fl = 1;
	else if (fl < 0)
		fl = 0;
}
Color hsv2rgb(float hue, float saturation, float brightness, int alpha)
{
	while (hue >= 1)
		hue -= 1;
	while (hue <= 0)
		hue += 1;

	ClampFl(saturation);
	ClampFl(brightness);

	float h = hue == 1.0f ? 0 : hue * 6.0f;
	float f = h - (int)h;
	float p = brightness * (1.0f - saturation);
	float q = brightness * (1.0f - saturation * f);
	float t = brightness * (1.0f - (saturation * (1.0f - f)));

	if (h < 1)
	{
		return Color(
			(unsigned char)(brightness * 255),
			(unsigned char)(t * 255),
			(unsigned char)(p * 255),
			alpha
		);
	}
	else if (h < 2)
	{
		return Color(
			(unsigned char)(q * 255),
			(unsigned char)(brightness * 255),
			(unsigned char)(p * 255),
			alpha
		);
	}
	else if (h < 3)
	{
		return Color(
			(unsigned char)(p * 255),
			(unsigned char)(brightness * 255),
			(unsigned char)(t * 255),
			alpha
		);
	}
	else if (h < 4)
	{
		return Color(
			(unsigned char)(p * 255),
			(unsigned char)(q * 255),
			(unsigned char)(brightness * 255),
			alpha
		);
	}
	else if (h < 5)
	{
		return Color(
			(unsigned char)(t * 255),
			(unsigned char)(p * 255),
			(unsigned char)(brightness * 255),
			alpha
		);
	}
	else
	{
		return Color(
			(unsigned char)(brightness * 255),
			(unsigned char)(p * 255),
			(unsigned char)(q * 255),
			alpha
		);
	}
}