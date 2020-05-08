#include "Esp.h"
#include "Engine.h"
#include "Entities.h"
#include "Draw.h"
#include "Aimbot.h"
#include "classid.h"
#include "MatSystem.h"
#include "Hooks.h"
#include "MatSystem.h"
#include "Materials.h"

Esp g_Esp;

#define strfind(x, y) x.find(y) != string::npos

bool Esp::Chams(CBaseEntity* Ent, DrawModelExecuteFn DrawModel, const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld)
{
	if (!Ent)
		return true;

	if (Ent->Dormant())
		return true;

	if (!me->Model())
		return true;

	if (!me->ActiveWep())
		return true;

	DWORD* model = (DWORD*)pInfo.pModel;

	string szModelName = g_Interfaces.ModelInfo->GetModelName(model);

	string szLocalModelName = g_Interfaces.ModelInfo->GetModelName(me->ActiveWep()->Model());

	int iLocalModelIndex = g_Interfaces.ModelInfo->GetModelIndex(szLocalModelName.c_str());
	int iModelIndex = g_Interfaces.ModelInfo->GetModelIndex(szModelName.c_str());

	if (g_Esp.chams_hand.value > 0 && strfind(szModelName, "arms") && iLocalModelIndex == iModelIndex)
	{
		if (g_Esp.chams_hand.value == 1)
			return false;

		Color col = chams_hand_color.color;
		
		Matptr mat = nullptr;

		switch (chams_hand.value)
		{
		case 2:
			mat = g_Mat.hand_shaded;
			break;
		case 3:
			mat = g_Mat.hand_glow;
			break;
		case 4:
			mat = g_Mat.hand_wireframe;
			break;
		case 5:
			mat = g_Mat.hand_shiny;
			break;
		}

		if (!mat)
			return true;

		mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
		g_Mat.ForceMaterial(chams_hand_color.color, mat);
	}

	if (Ent->fClientClass()->iClassID == (int)ent_id::CTFPlayer && g_Esp.chams_players.value)
	{
		Color col(255, 255, 255, 155);

		int team = Ent->TeamNum();

		if (team == RED_TEAM)
			col = red_clr.color;
		if (team == BLU_TEAM)
			col = blue_clr.color;

		Matptr mat = nullptr;

		switch (chams_material.value)
		{
		case 0:
			mat = g_Mat.shaded;
			break;
		case 1:
			mat = g_Mat.glow;
			break;
		case 2:
			mat = g_Mat.wireframe;
			break;
		case 3:
			mat = g_Mat.shiny;
			break;
		}

		if (mat == nullptr)
			return true;

		mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
		g_Mat.ForceMaterial(col, mat);

		DrawModel(g_Interfaces.ModelRender, state, pInfo, pCustomBoneToWorld);
		mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
	}

	if (strfind(szModelName, "med"))
	{
		Color col(0, 255, 0, 255);

		Matptr mat = nullptr;

		switch (chams_wep_material.value)
		{
		case 0:
			mat = g_Mat.shaded;
			break;
		case 1:
			mat = g_Mat.glow;
			break;
		case 2:
			mat = g_Mat.wireframe;
			break;
		case 3:
			mat = g_Mat.shiny;
			break;
		}

		if (mat == nullptr)
			return true;

		mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
		g_Mat.ForceMaterial(col, mat);
		DrawModel(g_Interfaces.ModelRender, state, pInfo, pCustomBoneToWorld);
		mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
	}

	if (strfind(szModelName, "buildables"))
	{

		Color col(255, 255, 255, 155);

		int team = Ent->TeamNum();

		if (team == RED_TEAM)
			col = red_clr.color;
		if (team == BLU_TEAM)
			col = blue_clr.color;


		Matptr mat = nullptr;

		switch (chams_wep_material.value)
		{
		case 0:
			mat = g_Mat.shaded;
			break;
		case 1:
			mat = g_Mat.glow;
			break;
		case 2:
			mat = g_Mat.wireframe;
			break;
		case 3:
			mat = g_Mat.shiny;
			break;
		}

		if (mat == nullptr)
			return true;

		mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
		g_Mat.ForceMaterial(col, mat);
		DrawModel(g_Interfaces.ModelRender, state, pInfo, pCustomBoneToWorld);
		mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
	}

	if (strfind(szModelName, "c_models") && chams_weapons.value && iLocalModelIndex != iModelIndex)
	{
		Color col = chams_wep_clr.color;

		Matptr mat = nullptr;

		switch (chams_wep_material.value)
		{
		case 0:
			mat = g_Mat.shaded;
			break;
		case 1:
			mat = g_Mat.glow;
			break;
		case 2:
			mat = g_Mat.wireframe;
			break;
		case 3:
			mat = g_Mat.shiny;
			break;
		}

		if (mat == nullptr)
			return true;

		mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
		g_Mat.ForceMaterial(col, mat);
		DrawModel(g_Interfaces.ModelRender, state, pInfo, pCustomBoneToWorld);
		mat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
	}
}

void Esp::Players()
{
	for (int i = 1; i < g_Interfaces.Engine->GetMaxClients(); i++)
	{
		if (i == g_Interfaces.Engine->GetLocalPlayer())
			continue;

		CBaseEntity* ent = GetEnt(i);

		if (!ent)
			continue;

		if (!master.value)
		{
			if (glow.value && ent->Glowing() == true)
			{
				ent->Glowing() = false;
				ent->UpdateGlowEffect();
			}
			return;
		}

		if (ent->Dormant())
		{
			if (glow.value && ent->Glowing() == true)
			{
				ent->Glowing() = false;
				ent->UpdateGlowEffect();
			}
			continue;
		}

		if (ent->LifeState() != LIFE_ALIVE)
		{
			if (glow.value && ent->Glowing() == true)
			{
				ent->Glowing() = false;
				ent->UpdateGlowEffect();
			}
			continue;
		}

		if (ent->TeamNum() == me->TeamNum() && !team.value)
		{
			if (glow.value && ent->Glowing() == true)
			{
				ent->Glowing() = false;
				ent->UpdateGlowEffect();
			}
			continue;
		}

		player_info_t info;
		if (!g_Interfaces.Engine->GetPlayerInfo(ent->Index(), &info))
			continue;

		const matrix3x4& vMatrix = ent->RgflCoordinateFrame();

		Vector vMin = ent->Mins();
		Vector vMax = ent->Maxs();

		Vector vPointList[] = {
			Vector(vMin.x, vMin.y, vMin.z),
			Vector(vMin.x, vMax.y, vMin.z),
			Vector(vMax.x, vMax.y, vMin.z),
			Vector(vMax.x, vMin.y, vMin.z),
			Vector(vMax.x, vMax.y, vMax.z),
			Vector(vMin.x, vMax.y, vMax.z),
			Vector(vMin.x, vMin.y, vMax.z),
			Vector(vMax.x, vMin.y, vMax.z)
		};

		Vector vTransformed[8];

		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 3; j++)
				vTransformed[i][j] = vPointList[i].Dot((Vector&)vMatrix[j]) + vMatrix[j][3];

		Vector flb, brt, blb, frt, frb, brb, blt, flt;

		if (!g_Draw.WorldToScreen(vTransformed[3], flb) ||
			!g_Draw.WorldToScreen(vTransformed[0], blb) ||
			!g_Draw.WorldToScreen(vTransformed[2], frb) ||
			!g_Draw.WorldToScreen(vTransformed[6], blt) ||
			!g_Draw.WorldToScreen(vTransformed[5], brt) ||
			!g_Draw.WorldToScreen(vTransformed[4], frt) ||
			!g_Draw.WorldToScreen(vTransformed[1], brb) ||
			!g_Draw.WorldToScreen(vTransformed[7], flt))
			continue;

		Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

		float left = flb.x;
		float top = flb.y;
		float right = flb.x;
		float bottom = flb.y;

		for (int i = 0; i < 8; i++)
		{
			if (left > arr[i].x)
				left = arr[i].x;
			if (top < arr[i].y)
				top = arr[i].y;
			if (right < arr[i].x)
				right = arr[i].x;
			if (bottom > arr[i].y)
				bottom = arr[i].y;
		}

		float x = left;
		float y = bottom;
		float w = right - left;
		float h = top - bottom;

		x += ((right - left) / 8); //pseudo fix for those THICC boxes
		w -= ((right - left) / 8) * 2;

		int iHp = ent->Health(), iMaxHp = ent->MaxHealth();
		if (iHp > iMaxHp)
			iHp = iMaxHp;

		Color entcol = Color(255, 255, 255, 255);

		if (ent->TeamNum() == BLU_TEAM)
			entcol = g_Esp.blue_clr.color;
		if (ent->TeamNum() == RED_TEAM)
			entcol = g_Esp.red_clr.color;

		Color text = text_color.color;

		if (g_Aimbot.showtarget.value && i == g_Aimbot.target_index)
			entcol = aim_color.color;

		int iY = 0;

		float percent = (float)ent->Health() / ent->MaxHealth();
		if (percent > 1) percent = 1;
		Color colhp = hsv2rgb(percent * 0.3f, 0.9f, 0.8f);

		if (glow.value)
		{
			if (ent->Glowing() == false)
			{
				ent->Glowing() = true;
				ent->UpdateGlowEffect();
			}
		}
		else
		{
			if (ent->Glowing() == true)
			{
				ent->Glowing() = false;
				ent->UpdateGlowEffect();
			}
		}

		if (box.value == 1)
		{
			g_Draw.OutlineRect(x - 1, y - 1, w + 2, h + 2, Color(0)); // inner outline
			g_Draw.OutlineRect(x, y, w, h, entcol); // inner entity color
			g_Draw.OutlineRect(x + 1, y + 1, w - 2, h - 2, Color(0)); // outer outline
		}

		if (box.value == 2)
		{
			g_Draw.OutlineRect(x - 1, y - 1, w + 2, h + 2, entcol); // inner outline
			g_Draw.OutlineRect(x, y, w, h, entcol); // inner entity color
		}

		if (name.value)
		{
			g_Draw.DrawString(x + w + 2, (y - 3) + iY, text, info.name, gFonts.esp);
			iY += g_Draw.GetESPHeight();
		}

		if (health.value == 1 || health.value == 3)
		{
			g_Draw.DrawString(x + w + 2, (y - 3) + iY, colhp, ToWchar(to_string(iHp).c_str()), gFonts.esp);
			iY += g_Draw.GetESPHeight();
		}

		if (szclass.value)
		{
			g_Draw.DrawString(x + w + 2, (y - 3) + iY, text, ent->szClass(), gFonts.esp);
			iY += g_Draw.GetESPHeight();
		}

		if (health.value == 2 || health.value == 3)
		{
			g_Draw.DrawRect(x - 6, y - 1, 5, h + 2, Color(0, 0, 0, 160));
			g_Draw.DrawRect(x - 5, y + (h - (h / iMaxHp * iHp)) - 1, 3, h / iMaxHp * iHp + 2, colhp);
		}
	}
}
