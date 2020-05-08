#include "Interfaces.h"
#include "Engine.h"
#include "Entities.h"
#include "Netvar.h"
#include "Crits.h"
#include "MD5.h"

Crits g_Crits;

// Game keeps track of these stats
struct stats_t
{
	float flCritBucket; // A54
	int   iNumAttacks;  // A58
	int   iNumCrits;    // A5C
} stats;

// Memory layout for C_TFWeaponBase
struct state_t
{
	bool  bCurrentAttackIsCrit;
	bool  bLowered;                  // NETVAR 0xB40
	int   iAltFireHint;              // 0xB44
	int   iReloadStartClipAmount;    // 0xB48
	float flCritEndTime;             // 0xB4C
	float flLastCritCheckTime;       // NETVAR // 0xB50
	int   iLastCritCheckFrame;       // 0xB54
	int   iCurrentSeed;              // 0xB58
	float flLastCritCheckTimeNoPred; // 0xB5C
};

state_t &GetState(CBaseCombatWeapon* pWeapon)
{
	static int offset = g_Netvar.get_offset("DT_TFWeaponBase", "m_bLowered") - 0x4;
	return *(state_t*)(pWeapon + offset);
}

void Crits::CorrectBucket(CBaseEntity *pWeapon, CUserCmd* pCommand)
{
	static bool changed;
	static float last_bucket;
	static int last_weapon;

	if (pCommand->command_number)
		changed = false;

	stats_t stats = *(stats_t*)(pWeapon + 0xA54);

	float &bucket = stats.flCritBucket;

	if (bucket != last_bucket)
	{
		if (changed && pWeapon->Index() == last_weapon)
		{
			bucket = last_bucket;
		}
		changed = true;
	}
	last_weapon = pWeapon->Index();
	last_bucket = bucket;
}

int Crits::FindNextCritCmdN(CBaseEntity *pWeapon, CUserCmd* pCommand)
{
	int tries = 0, number = pCommand->command_number, found = 0, seed_backup;

	state_t state;
	stats_t stats;

	seed_backup = *g_Interfaces.g_PredictionRandomSeed;

	memcpy(&state, &GetState((CBaseCombatWeapon*)pWeapon), sizeof(state_t));
	memcpy(&stats, &*(stats_t*)(pWeapon + 0xA54), sizeof(stats_t));
	bShouldEnginePredict = false;
	seed_backup = *g_Interfaces.g_PredictionRandomSeed;
	while (!found && tries < 4096)
	{
		*g_Interfaces.g_PredictionRandomSeed = MD5_PseudoRandom(number) & 0x7FFFFFFF;
		found = me->ActiveWep()->CalcIsCrit();

		if (found)
		{
			break;
		}

		++tries;
		++number;
	}

	*g_Interfaces.g_PredictionRandomSeed = seed_backup;


	memcpy(&GetState((CBaseCombatWeapon*)pWeapon), &state, sizeof(state_t));
	memcpy(&*(stats_t*)(pWeapon + 0xA54), &stats, sizeof(stats_t));
	bShouldEnginePredict = true;

	if (found)
		return number;

	return 0;
}

struct cached_calculation_s
{
	int command_number;
	int init_command;
	int weapon_entity;
};

static cached_calculation_s cached_calculation{};

#include "Aimbot.h"

bool Crits::ForceCrit(CBaseCombatWeapon* pWeapon, CUserCmd* pCmd)
{
	auto command_number = pCmd->command_number;

	if (LastCmdN < command_number || LastWeapon != pWeapon->Model() || LastCmdN - command_number > 1000)
	{
		//if (cached_calculation.init_command > command_number || command_number - cached_calculation.init_command > 50 || (command_number && (cached_calculation.command_number < command_number)))
		//	cached_calculation.weapon_entity = 0;
		//if (cached_calculation.weapon_entity == pWeapon->Index())
		//	return bool(cached_calculation.command_number);

		CmdN = FindNextCritCmdN(pWeapon, pCmd);
	}
	else
		CmdN = LastCmdN;

	//CritStatus = "Found crit at: " + to_string(CmdN);

	//if (command_number != CmdN && CmdN && CmdN != command_number) {
	//	pCmd->buttons &= ~IN_ATTACK;
	//	pCmd->buttons &= ~IN_RELOAD;
	//	CritStatus = "Waiting...";
	//}
	//else {
	//	pCmd->buttons &= ~IN_RELOAD;
	//	pCmd->buttons |= IN_ATTACK;
	//	CritStatus = "Fired!";
	//}

	if (CmdN && CmdN != command_number && me->ActiveWep()->GetSlot() != 2)
	{
		new_command_number[command_number] = CmdN;
		CNetChannel* net = g_Interfaces.Engine->GetNetChannelInfo();
		net->m_nOutSequenceNr = CmdN - 1;
	}

	LastWeapon = pWeapon->Model();
	LastCmdN = CmdN;

	return CmdN != 0;
}

#include "EnginePred.h"

void Crits::Tick(CUserCmd *pCommand)
{
	LastUserCmd = pCommand->command_number;

	if (g_Pred.bPredicting)
		return;

	if (!g_Aimbot.CanShoot)
		return;

	CBaseCombatWeapon* pActiveWep = me->ActiveWep();
	CorrectBucket(pActiveWep, pCommand);
	if ((pCommand->buttons & IN_ATTACK) && crit_key.KeyDown())
	{
		bShouldEnginePredict = false;
		ForceCrit(pActiveWep, pCommand);
	}
	else { bShouldEnginePredict = true; }
}

void Crits::Frame()
{
	return;
	//if (!g_Interfaces.Engine->IsInGame())
	//	return;

	//CBaseCombatWeapon* pActiveWep = me->ActiveWep();

	//if (!pActiveWep)
	//	return;

	//static int x = 200, y = 200;

	//static bool dragging = false;

	//if (g_Menu.mouseOver(x, y, 175, 75))
	//{
	//	if (g_Menu.mb == e_mb::lclick)
	//		dragging = true;
	//	else if (g_Menu.mb != e_mb::ldown)
	//		dragging = false;
	//}

	//if (dragging)
	//{
	//	x += g_Menu.mouse.x - g_Menu.pmouse.x;
	//	y += g_Menu.mouse.y - g_Menu.pmouse.y;
	//}

	//g_Draw.DrawRect(x, y - 10, 175, 10, Color(26, 28, 32, 255)); // top bar
	//g_Draw.DrawRect(x, y, 175, 75, Color(31, 31, 37, 255)); // background


	//if (crit_key.KeyDown())
	//{

	//	g_Draw.OutlineRect(x + 5, y + 60, 165, 10, bCanCrit ? Color(29, 136, 231, 150) : Color(255, 0, 0, 150));
	//}

	//stats_t stats = *(stats_t*)(pActiveWep + 0xA54);

	//int bucket = (int)(stats.flCritBucket);

	//int wide = (bucket / 3.75 - bucket * .1) - 1;

	//g_Draw.DrawString(x + wide, (y + 45) - 12, Color(181, 181, 181), to_string(bucket));
	//g_Draw.DrawRect(x + 5, y + 45, wide, 10, Color(29, 136, 231));

	//int w, h;

	//g_Interfaces.Surface->GetTextSize(gFonts.esp, ToWchar(CritStatus.c_str()).c_str(), w, h);

	//g_Draw.DrawString(x + ((175 / 2) + (w / 2)) - w, y + 10, Color(181, 181, 181), CritStatus);

	// //g_Draw.DrawString(x, y + 20, Color(255), string("Bucket: " + to_string(bucket)));

	//if (LastUserCmd)
	//{
	//	if (CmdN > LastUserCmd)
	//	{
	//		float nextcrit = ((float)CmdN - (float)LastUserCmd) / (float)90; // seconds
	//		if (nextcrit > 0.0f)
	//		{

	//			int wide = nextcrit * 165;

	//			if (wide > 165)
	//				wide = 165;

	//			g_Draw.DrawString(x + wide, y + 48, Color(181, 181, 181), to_string((nextcrit * 100 + .5) / 100).substr(1, 3));
	//			g_Draw.DrawRect(x + 5, y + 60, wide, 10, Color(29, 136, 231));
	//		}
	//	}
	//}
}