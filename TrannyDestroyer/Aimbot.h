#pragma once

#include "Controls.h"

class CUserCmd;
class CBaseEntity;

class Aimbot
{
public:

	void Tick(CBaseEntity* pLocal, CUserCmd* pCommand);
	bool CalcCanShoot(CBaseEntity * You);

	Checkbox master = Checkbox("Enabled");
	Checkbox silent = Checkbox("Silent");
	Checkbox hitscan = Checkbox("Hitscan");
	Checkbox autoshoot = Checkbox("Auto-shoot");
	Slider fov = Slider("Fov Limit", 90, 0, 180, 1);
	Checkbox proj = Checkbox("Projectile Compensation");
	Checkbox ignore_cloak = Checkbox("Ignore Cloaked", true);
	KeyBind aimkey = KeyBind("Aim Key", 'F');
	Checkbox scope = Checkbox("Scoped aim only");
	Listbox aim_option = Listbox("Aim Option", { "Auto", "Head", "Body", "Hitscan" });
	Checkbox showtarget = Checkbox("Show Aimbot Target");

	bool CanShoot;
	int target_index;
	int last_target = -1;
private:
	int FindTarget();
	int FindBestHitbox(CBaseEntity* pEntity);
	Vector ProjectileCompensate(CBaseEntity* pEntity, float quality);
};

extern Aimbot g_Aimbot;