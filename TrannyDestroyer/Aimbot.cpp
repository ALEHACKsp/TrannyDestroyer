#include "Aimbot.h"
#include "Interfaces.h"
#include "Base.h"
#include "Entities.h"
#include "Trace.h"
#include "Hit.h"
#include "ICvar.h"
#include "Convar.h"

Aimbot g_Aimbot;

int ProjSpeed(CBaseEntity* weapon);

Vector AnglesTo(const Vector& Pos)
{
	if (!me)
		return Vector();

	Vector angles; // Converting that line direction to angles
	VectorAngles(Pos - me->EyePos(), angles);
	ClampAngle(angles);

	return angles;
}

void Aimbot::Tick(CBaseEntity* pLocal, CUserCmd* cmd)
{
	g_Aimbot.target_index = -1;

	if (!master.value)
		return;

	if (!aimkey.KeyDown())
		return;

	if (!me->ActiveWep())
		return;

	int index = me->ActiveWep()->DefIndex();
	if (scope.value && me->ClassNum() == TF2_Sniper && !(me->Cond() & TFCond_Zoomed) && (index == WPN_SniperRifle ||
		index == WPN_Machina || index == WPN_SydneySleeper || index == WPN_FestiveSniperRifle || index == WPN_AWP))
		return;

	CBaseEntity* pEntity = GetEnt(FindTarget());

	if (!pEntity)
		return;

	int iBestHitbox = FindBestHitbox(pEntity);

	if (iBestHitbox == -1)
		return;
	Vector vEntity;

	int iSpeed = ProjSpeed(me->ActiveWep());

	if (iSpeed != -1 && proj.value) {
		float flTime = (me->EyePos() - pEntity->AbsOrigin()).Length() / iSpeed;

		for (int i = 0; i < 4; i++)
		{
			vEntity = ProjectileCompensate(pEntity, flTime);
			flTime = (me->EyePos() - vEntity).Length() / iSpeed;
		}
		if (!g_Hit.AssumeVis(me, me->EyePos(), vEntity, nullptr))
			return;
	}
	else
		vEntity = pEntity->HitboxPosition(iBestHitbox);


	if (iBestHitbox == 0 && !proj.value)
	{
		trace_t Trace;
		Ray_t Ray;
		CTraceFilterCustom Filter(me);

		Vector s = me->EyePos(), e = pEntity->HitboxPosition(iBestHitbox);

		Ray.Init(s, e);

		g_Interfaces.Trace->TraceRay(Ray, MASK_SHOT, &Filter, &Trace);

		if (Trace.hitbox != 0)
			vEntity.z += 3;
	}

	Vector vLocal = pLocal->EyePos();

	Vector vAngs;
	VectorAngles((vEntity - vLocal), vAngs);
	ClampAngle(vAngs);

	g_Aimbot.target_index = pEntity->Index();
	g_Aimbot.last_target = pEntity->Index();

	cmd->viewangles = vAngs;

	if(!silent.value)
		g_Interfaces.Engine->SetViewAngles(cmd->viewangles);

	if (autoshoot.value)
	{
		cmd->buttons |= IN_ATTACK;
	}
}

int Aimbot::FindTarget()
{
	int Index = -1;
	//this num could be smaller 
	float lowestDist = 181.0f;

	Vector vLocal = me->EyePos();
	Vector vecClient = g_Interfaces.Engine->GetViewAngles(), vecFinal = vecClient;

	for (int i = 1; i <= g_Interfaces.Engine->GetMaxClients(); i++)
	{
		if (i == me->Index())
			continue;

		CBaseEntity* pEntity = GetEnt(i);

		if (!pEntity)
			continue;

		if (pEntity->Dormant())
			continue;

		if (pEntity->LifeState() != LIFE_ALIVE ||
			pEntity->TeamNum() == me->TeamNum())
			continue;

		int iBestHitbox = FindBestHitbox(pEntity);

		if (iBestHitbox == -1)
			continue;

		Vector vEntity = pEntity->HitboxPosition(iBestHitbox); //pEntity->GetWorldSpaceCenter(vEntity)

		if (pEntity->Cond() & TFCond_Ubercharged ||
			pEntity->Cond() & TFCond_UberchargeFading ||
			pEntity->Cond() & TFCond_Bonked)
			continue;

		if (ignore_cloak.value && pEntity->Cond() & TFCond_Cloaked)
			continue;

		Vector angleDiff = AnglesTo(vEntity) - vecClient;

		if (angleDiff.y > 180)
			angleDiff.y -= 360;
		else if (angleDiff.y < -180)
			angleDiff.y += 360;

		// Check if it's within our FOV range
		float distance = angleDiff.Length2D();
		if (distance <= fov.value)
		{
			// Look for lower FOV distance than last target
			if (distance < lowestDist)
				Index = i, lowestDist = distance, vecFinal = vEntity;
		}
	}

	return Index;
}

int Aimbot::FindBestHitbox(CBaseEntity* ent)
{
	int best = -1;

	if (aim_option.value == 0)
	{
		switch (me->ActiveWep()->DefIndex())
		{
		case WPN_Ambassador:
		case WPN_SniperRifle:
		case WPN_AWP:
		case WPN_Machina:
		case WPN_ShootingStar:
		case WPN_FestiveAmbassador:
		case WPN_FestiveSniperRifle:
			best = 0;
			break;

		default:
			for (int h = 1; h <= 5; h++)
			{
				if (g_Hit.CanHit(ent, me->EyePos(), ent->HitboxPosition(h)))
				{
					best = h;
					break;
				}
			}
			break;
		}
	}

	if (aim_option.value == 1)
	{
		if (g_Hit.CanHit(ent, me->EyePos(), ent->HitboxPosition(0)))
		{

			return 0;
		}
	}

	if (aim_option.value == 2)
	{
		for (int h = 1; h <= 5; h++)
		{
			if (g_Hit.CanHit(ent, me->EyePos(), ent->HitboxPosition(h)))
			{
				return h;
			}
		}
	}

	if (aim_option.value == 3)
	{
		for (int i = 0; i < 17; i++)
		{
			if (g_Hit.CanHit(ent, me->EyePos(), ent->HitboxPosition(i)))
			{
				best = i;
				break;
			}
		}
	}

	if (best == -1)
		return -1;

	if (ent->HitboxPosition(best).IsZero())
		return -1;

	if (!g_Hit.CanHit(ent, me->EyePos(), ent->HitboxPosition(best)))
		return -1;

	return best;
}


int ProjSpeed(CBaseEntity* weapon)
{
	if (weapon == NULL)
		return -1;
	ent_id name = (ent_id)weapon->fClientClass()->iClassID;
	if (name == ent_id::CTFRocketLauncher_DirectHit)
		return 1980;
	else if (name == ent_id::CTFRocketLauncher_AirStrike ||
		name == ent_id::CTFParticleCannon ||
		name == ent_id::CTFRocketLauncher)
		return 1100;
	else if (name == ent_id::CTFRaygun)
		return 840;
	return -1; // Return this so we know if the weapon isn't supported
}

Vector Aimbot::ProjectileCompensate(CBaseEntity* pEntity, float flTime)
{
	static ConVar* sv_gravity = g_Interfaces.Cvar->FindVar("sv_gravity");

	int gravity = sv_gravity->GetInt();

	Vector vStart = pEntity->AbsOrigin(), vVelocity = pEntity->AbsVelocity();

	float zdrop = pEntity->Flags() & FL_ONGROUND ? vVelocity.z * flTime : 0.5 * -gravity * pow(flTime, 2) + vVelocity.z * flTime;

	Vector result(
		vStart.x + (vVelocity.x * flTime),
		vStart.y + (vVelocity.y * flTime),
		vStart.z + zdrop);

	return result;
}

bool Aimbot::CalcCanShoot(CBaseEntity* You)
{
	CBaseCombatWeapon* wep = You->ActiveWep();
	if (!wep)
		return false;

	static float flNext = 0, flLast = 0;
	static CBaseCombatWeapon* pLastWep;

	if (flLast != wep->LastFireTime() || wep != pLastWep)
		flLast = wep->LastFireTime(), flNext = wep->NextPrimaryAttack();

	if (!wep->Clip1())
		return false;

	pLastWep = wep;

	return CanShoot = flNext <= float(me->Tickbase()) * g_Interfaces.Globals->interval_per_tick;
}