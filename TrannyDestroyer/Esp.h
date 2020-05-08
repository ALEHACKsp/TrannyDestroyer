#pragma once
#include "Controls.h"
#include "Hooks.h"

class CBaseEntity;

class Esp
{
public:
	bool Chams(CBaseEntity* Ent,
		DrawModelExecuteFn DrawModel,
		const DrawModelState_t &state,
		const ModelRenderInfo_t &pInfo,
		matrix3x4 *pCustomBoneToWorld);
	void Players();

	Checkbox master = Checkbox("Enabled");
	Checkbox team = Checkbox("Draw Team");

	Checkbox name = Checkbox("Name");
	Listbox health = Listbox("Health", {"None", "Value", "Bar", "Both"});
	Checkbox szclass = Checkbox("Class");

	Listbox box = Listbox("Box Style", {"None", "Outline", "No Outline"});
	Checkbox glow = Checkbox("Glow");

	Checkbox misc_noscope = Checkbox("No Scope");
	Checkbox misc_nozoom = Checkbox("No Zoom");
	Checkbox misc_drawvmodel = Checkbox("Draw View Model", true);
	Checkbox misc_gold_weps = Checkbox("Gold Weapons");

	Checkbox chams_enabled = Checkbox("Enabled");
	Checkbox chams_players = Checkbox("Players");
	Checkbox chams_buildings = Checkbox("Buildings");
	Checkbox chams_team = Checkbox("Draw Team");
	Checkbox chams_weapons = Checkbox("Weapons");

	Listbox chams_hand = Listbox("Hand Chams", {"Default", "No Draw", "Shaded", "Glow", "Wireframe", "Shiny" });
	ColorPicker chams_hand_color = ColorPicker("Hand Chams Color", Color(255));
	Listbox chams_material = Listbox("Player Material", {"Shaded", "Glow", "Wireframe", "Shiny"});
	Listbox chams_wep_material = Listbox("Weapon Material", { "Shaded", "Glow", "Wireframe", "Shiny" });

	ColorPicker red_clr = ColorPicker("Red Team Color", Color(220, 20, 60));
	ColorPicker blue_clr = ColorPicker("Blue Team Color", Color(30, 144, 255));

	ColorPicker chams_wep_clr = ColorPicker("Weapon Color", Color(255));

	ColorPicker aim_color = ColorPicker("Aim Target color", Color(0, 255, 0));
	ColorPicker text_color = ColorPicker("Text Color", Color(255));
};

extern Esp g_Esp;