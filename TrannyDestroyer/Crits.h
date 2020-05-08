#pragma once
#include "Controls.h"
#include <string>
#include <deque>
#include <unordered_map>

class CUserCmd;
class CBaseEntity;
class CBaseCombatWeapon;

class Crits
{
public:
	void Tick(CUserCmd *pCommand);
	void Frame();

	Checkbox melee_crits = Checkbox("Melee Crits");
	KeyBind crit_key = KeyBind("Crit Key", 'F', false);

	std::unordered_map<int, int> new_command_number;

	bool bShouldEnginePredict = false;
private:
	bool ForceCrit(CBaseCombatWeapon* pWeapon, CUserCmd* pCmd);
	void CorrectBucket(CBaseEntity* weapon, CUserCmd* pCommand);
	int FindNextCritCmdN(CBaseEntity* pWeapon, CUserCmd * pCommand);

	int CmdN = 0;
	int LastCmdN = 0;
	int LastUserCmd = 0;

	unsigned long* LastWeapon = nullptr;

	//string CritStatus = "None";
};

extern Crits g_Crits;