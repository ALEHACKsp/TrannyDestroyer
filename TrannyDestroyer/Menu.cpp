#include "Menu.h"
#include "Draw.h"
#include "Esp.h"
#include "Aimbot.h"
#include "Misc.h"
#include "Triggerbot.h"
#include "Antiaim.h"
#include "Crits.h"

Menu g_Menu;

#define keyDown GetAsyncKeyState
#define keyPress(x) keyDown(x) & 1
CKey gKey;


#define GROUP_WIDTH 180
#define TAB_WIDTH 150
#define MENU_TOPBAR 30

//#include "Editor.h"
void Menu::CreateGUI()
{

	Tabs.AddTab(new Tab("Aimbot", {
	&g_Aimbot.master,
	&g_Aimbot.aimkey,

		&g_Aimbot.autoshoot,
		&g_Aimbot.proj,
		&g_Aimbot.aim_option,
		&g_Aimbot.silent,
		&g_Aimbot.fov,
		&g_Aimbot.ignore_cloak,
		&g_Aimbot.scope,
		&g_Aimbot.showtarget,

	}, TAB_WIDTH));

	Tabs.AddTab(new Tab("Triggerbot", {
	&g_Triggerbot.master,
	&g_Triggerbot.trigkey,
	&g_Triggerbot.scoped,
	&g_Triggerbot.ignore_cloak,
	&g_Triggerbot.ignore_disguise,
	&g_Triggerbot.hitbox,
	}, TAB_WIDTH));


	/*Tabs.AddTab(new Tab("HvH", {
		&g_AntiAim.master,
		&g_AntiAim.yaw_bsendpacket,
		new Space(200),
		new Groupbox("AntiAim", {
		&g_AntiAim.pitch,
		&g_AntiAim.pitch_bsendpacket,
		&g_AntiAim.real_pitch,
		&g_AntiAim.fake_pitch,
		&g_AntiAim.yaw,
		&g_AntiAim.real_yaw,
		&g_AntiAim.fake_yaw,
			}, GROUP_WIDTH),

	}, TAB_WIDTH));*/

	Tabs.AddTab(new Tab("Esp", {
		&g_Esp.master,
		&g_Esp.team,

		new Groupbox("Players", {

			&g_Esp.name,
			&g_Esp.szclass,
			&g_Esp.health,
			&g_Esp.box,
			&g_Esp.glow,

		}, GROUP_WIDTH),

		new Groupbox("Chams", {
			&g_Esp.chams_enabled,
			&g_Esp.chams_players,
			&g_Esp.chams_team,
			&g_Esp.chams_weapons,
			&g_Esp.chams_material,
			&g_Esp.chams_wep_material,
			new Space(5),
			&g_Esp.chams_hand,
			&g_Esp.chams_hand_color,
		}, GROUP_WIDTH),

		new Groupbox("Misc", {
			&g_Esp.misc_noscope,
			&g_Esp.misc_nozoom,
			&g_Esp.misc_drawvmodel,
		}, GROUP_WIDTH),


		new Groupbox("Colors", {
			&g_Esp.aim_color,
			&g_Esp.text_color,

			&g_Esp.red_clr,
			&g_Esp.blue_clr,
			&g_Esp.chams_wep_clr,
		}, GROUP_WIDTH),

	}, TAB_WIDTH));

	Tabs.AddTab(new Tab("Misc", {
		&g_Misc.bhop,
		&g_Misc.autostrafe,
		&g_Misc.enginepred,
		//&g_Crits.melee_crits,
		&g_Crits.crit_key,
	}, TAB_WIDTH));
}

#include "XorString.h"

void Menu::Draw()
{
	if (key == VK_INSERT || key == VK_F11)
		enabled = !enabled;

	g_Draw.DrawString(50, 110, Color(181, 181, 181, 5), XorStr("trannydestroyer uc build"), gFonts.verdana_bold);

	if (!enabled)
		return;

	static bool dragging = false;

	if (mb == e_mb::lclick && mouseOver(pos.x, pos.y, scale.x, MENU_TOPBAR))
		dragging = true;
	else if (mb != e_mb::ldown)
		dragging = false;

	if (dragging && focus == 0)
	{
		pos.x += mouse.x - pmouse.x;
		pos.y += mouse.y - pmouse.y;
	}

	for (auto& window : Windows)
	{
		if (!dragging && !focus)
			window->HandleInput();
		window->Draw();
	}

	POINT _pos = pos, _scale = scale;
	_scale.y += MENU_TOPBAR;

#pragma region Main window
	// Menu outline
	g_Draw.OutlineRect(pos.x - 1, pos.y - 1, _scale.x + 2, _scale.y + 2, Color(0));
	int topbar = style->TopBar(_pos.x, _pos.y, _scale.x, "TrannyDestroyer - by yapht");

	// Re-adjusting pos and scale for easy coding
	_pos.y += topbar, _scale.y -= topbar;

	// Tab region
	g_Draw.DrawRect(_pos.x, _pos.y, TAB_WIDTH, _scale.y, Color(22, 23, 24));
	g_Draw.DrawLine(_pos.x + TAB_WIDTH - 1, _pos.y, _pos.x + TAB_WIDTH - 1, _pos.y + _scale.y, Color(0));

	Tabs.SetPos(_pos.x, _pos.y + topbar);
	Tabs.SetWidth(TAB_WIDTH);
	Tabs.HandleInput();
	Tabs.Draw(false);

	// Control region
	g_Draw.DrawRect(_pos.x + TAB_WIDTH, _pos.y, _scale.x - TAB_WIDTH, _scale.y, Color(36, 36, 42));
	// Dividing line
	g_Draw.DrawLine(_pos.x, _pos.y, _pos.x + _scale.x, _pos.y, Color(0));
	// Re-adjusting pos and scale again
	_pos.x += TAB_WIDTH + 3, _scale.x = scale.x - (_pos.x - pos.x);

#pragma endregion

	if (Tabs.active)
	{
		int cx = _pos.x + 13, cy = _pos.y + 12;
		int maxWidth = 0;
		auto& controls = Tabs.active->GetChildren();
		for (size_t i = 0; i < controls.size(); i++)
		{
			if (!controls[i]->IsVisible())
				continue;

			if (cy + controls[i]->GetHeight() > scale.y + _pos.y - 12)
				cy = _pos.y + 12, cx += 13 + maxWidth + 10, maxWidth = 0;

			if (controls[i]->GetWidth() > maxWidth)
				maxWidth = controls[i]->GetWidth();
			controls[i]->SetPos(cx, cy);

			bool over = mouseOver(cx, cy, controls[i]->GetWidth(), controls[i]->GetHeight());
			bool getInput = !(controls[i]->flags & noinput) && over && !IsDialogOpen();
			if (getInput)
				controls[i]->HandleInput();

			cy += controls[i]->Draw(getInput) + SPACING;
		}
	}

	size_t last = dialogs.size() - 1;
	if (dialogs.size() > 1)
	{
		e_mb new_mb = mb;
		e_mw new_mw = mw;
		POINT new_mouse = mouse, new_pmouse = pmouse;

		// Enforce focus so that only the last dialog gets to use these variables
		mb = e_mb::null, mw = e_mw::null, mouse = pmouse = { 0, 0 };

		for (size_t i = 0; i < last; i++)
		{
			if (dialogs[i] == nullptr)
				continue;

			dialogs[i]->Draw(dialogs[i]->data, i + 1);
		}
		mb = new_mb, mw = new_mw, mouse = new_mouse, pmouse = new_pmouse;
		dialogs[last]->Draw(dialogs[last]->data, last + 1);
	}
	else if (!last)
		dialogs[last]->Draw(dialogs[last]->data, last + 1);

	if (key == VK_ESCAPE && dialogs.size())
		dialogs.pop_back();
}

bool Menu::mouseOver(int x, int y, int w, int h)
{
	return mouse.x >= x && mouse.x <= x + w && mouse.y >= y && mouse.y <= y + h;
}

bool Menu::mouseOver(CBounds region)
{
	return mouse.x >= region.x && mouse.x <= region.x + region.w && mouse.y >= region.y && mouse.y <= region.y + region.h;
}

void Menu::GetInput()
{
	int mx = 0, my = 0;
	g_Interfaces.Surface->GetCursorPosition(mx, my);

	pmouse = mouse;
	mouse = { mx, my };

	if (keyDown(VK_LBUTTON))
	{
		if (mb == e_mb::lclick || mb == e_mb::ldown)
			mb = e_mb::ldown;
		else
			mb = e_mb::lclick;
	}
	else if (keyDown(VK_RBUTTON))
	{
		if (mb == e_mb::rclick || mb == e_mb::rdown)
			mb = e_mb::rdown;
		else
			mb = e_mb::rclick;
	}
	else
		mb = e_mb::null;
}

void Menu::EndInput()
{
	// Reseting Window message variables so they won't stick
	mw = e_mw::null;
	key = NULL;
}

LRESULT __stdcall Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_MOUSEWHEEL:
		if ((int)wParam < 0)
			g_Menu.mw = e_mw::up;
		else
			g_Menu.mw = e_mw::down;
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN: // Allows input to work with alt key
		if (wParam > 255)
			break;
		g_Menu.keys[wParam] = true, g_Menu.last_key = wParam = g_Menu.key = wParam;
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		// Avoid Mouse 3/4/5 here
		if (wParam > 255 && wParam < VK_MBUTTON && wParam > VK_XBUTTON2)
			break;
		g_Menu.keys[wParam] = false;
		if (g_Menu.last_key == wParam)
			g_Menu.last_key = NULL;
		break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
		g_Menu.keys[VK_MBUTTON] = uMsg == WM_MBUTTONDOWN;
		if (uMsg == WM_MBUTTONDOWN)
			g_Menu.last_key = g_Menu.key = VK_MBUTTON;
		else if (g_Menu.last_key == VK_MBUTTON)
			g_Menu.last_key = NULL;
		break;
		/*case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
			int param = GET_KEYSTATE_WPARAM(wParam);
			if (!(param & MK_XBUTTON1) && !(param & MK_XBUTTON2))
				break;

			byte vk;
			if (param & MK_XBUTTON1)
			{
				vk = VK_XBUTTON1;
				gMenu.keys[vk] = uMsg == WM_XBUTTONDOWN;
				if (uMsg == WM_XBUTTONDOWN)
					gMenu.last_key = gMenu.key = vk;
				else if (gMenu.last_key == vk)
					gMenu.last_key = NULL;
			}
			if (param & MK_XBUTTON2)
			{
				vk = VK_XBUTTON2;
				gMenu.keys[vk] = uMsg == WM_XBUTTONDOWN;
				if (uMsg == WM_XBUTTONDOWN)
					gMenu.last_key = gMenu.key = vk;
				else if (gMenu.last_key == vk)
					gMenu.last_key = NULL;
			}*/
	}

	if (g_Menu.enabled && g_Menu.keys[VK_ESCAPE])
		return 0; // Don't let escape key affect the game while using our UI

	return CallWindowProc(g_Menu.windowProc, hWnd, uMsg, wParam, lParam);
}

void Menu::OpenDialog(Dialog& dlg)
{
	dialogs.push_back(&dlg);
	focus = dialogs.size();
}
void Menu::CloseDialog(size_t Index)
{
	if (Index == 0)
		return;

	Index--;
	if (Index >= dialogs.size())
		return;

	dialogs.erase(dialogs.begin() + Index);
	focus = dialogs.size();
}