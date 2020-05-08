#pragma once
#include "Base.h"
#include "Vector.h"
#include "getvfunc.h"
#include "Engine.h"
#include "Netvar.h"
#include "Interfaces.h"
#include "Signature.h"


typedef struct player_info_s
{
	char			name[32];
	int				userID;
	char			guid[33];
	unsigned long	friendsID;
	char			friendsName[32];
	bool			fakeplayer;
	bool			ishltv;
	unsigned long	customFiles[4];
	unsigned char	filesDownloaded;
} player_info_t;

class CBaseCombatWeapon;

#define STUDIO_NONE						0x00000000
#define STUDIO_RENDER					0x00000001
#define STUDIO_VIEWXFORMATTACHMENTS		0x00000002
#define STUDIO_DRAWTRANSLUCENTSUBMODELS 0x00000004
#define STUDIO_TWOPASS					0x00000008
#define STUDIO_STATIC_LIGHTING			0x00000010
#define STUDIO_WIREFRAME				0x00000020
#define STUDIO_ITEM_BLINK				0x00000040
#define STUDIO_NOSHADOWS				0x00000080
#define STUDIO_WIREFRAME_VCOLLIDE		0x00000100

// Not a studio flag, but used to flag model as a non-sorting brush model
#define STUDIO_TRANSPARENCY				0x80000000

// Not a studio flag, but used to flag model as using shadow depth material override
#define STUDIO_SHADOWDEPTHTEXTURE	0x40000000

class CBaseEntity
{
public:
	Vector& AbsOrigin()
	{
		typedef Vector& (__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 9)(this);
	}

	Vector& AbsAngles()
	{
		typedef Vector& (__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 10)(this);
	}

	Vector WorldSpaceCenter()
	{
		Vector vWorldSpaceCenter = Vector(0, 0, 0);
		Vector vMin,vMax;
		this->RenderBounds(vMin, vMax);
		vWorldSpaceCenter = this->AbsOrigin();
		vWorldSpaceCenter.z += (vMin.z + vMax.z) / 2;
	}

	Vector Mins()
	{
		DYNVAR_RETURN(Vector, this, "DT_BaseEntity", "m_Collision", "m_vecMins");
	}

	Vector Maxs()
	{
		DYNVAR_RETURN(Vector, this, "DT_BaseEntity", "m_Collision", "m_vecMaxs");
	}

	Vector EyePos()
	{
		DYNVAR_RETURN(Vector, this, "DT_BasePlayer", "localdata", "m_vecViewOffset[0]") + this->AbsOrigin();
	}

	Vector AbsEyePos()
	{
		DYNVAR_RETURN(Vector, this, "DT_BasePlayer", "localdata", "m_vecViewOffset[0]");
	}

	Vector& EyeAngles()
	{
		static int offset = g_Netvar.get_offset("DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[0]");
		return *(Vector*)(this + offset);
	}

	Vector HitboxPosition(int iHitbox)
	{
		DWORD *model = this->Model();
		if (!model)
			return Vector();

		studiohdr_t *hdr = g_Interfaces.ModelInfo->GetStudiomodel(model);
		if (!hdr)
			return Vector();

		matrix3x4 matrix[128];
		if (!this->SetupBones(matrix, 128, 0x100, 0))
			return Vector();

		int HitboxSetIndex = *(int *)((DWORD)hdr + 0xB0);
		if (!HitboxSetIndex)
			return Vector();

		mstudiohitboxset_t *pSet = (mstudiohitboxset_t *)(((PBYTE)hdr) + HitboxSetIndex);

		mstudiobbox_t* box = pSet->pHitbox(iHitbox);
		if (!box)
			return Vector();

		Vector vCenter = (box->bbmin + box->bbmax) * 0.5f;

		Vector vHitbox;

		VectorTransform(vCenter, matrix[box->bone], vHitbox);

		return vHitbox;
	}

	Vector AbsVelocity()
	{
		typedef void(__thiscall * EstimateAbsVelocityFn)(CBaseEntity *, Vector &);
		static DWORD dwFunc = g_Sig.Find("client.dll", "E8 ? ? ? ? F3 0F 10 4D ? 8D 85 ? ? ? ? F3 0F 10 45 ? F3 0F 59 C9 56 F3 0F 59 C0 F3 0F 58 C8 0F 2F 0D ? ? ? ? 76 07");

		static bool init = false;
		if (!init)
			dwFunc = ((*(PDWORD)(dwFunc + 1)) + dwFunc + 5); init = true;

		static EstimateAbsVelocityFn vel = (EstimateAbsVelocityFn)dwFunc;

		Vector v;
		vel(this, v);

		return v;
	}

	DWORD* Model()
	{
		PVOID pRenderable = (PVOID)(this + 0x4);
		typedef DWORD* (__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pRenderable, 9)(pRenderable);
	}

	bool SetupBones(matrix3x4 *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
	{
		PVOID pRenderable = (PVOID)(this + 0x4);
		typedef bool(__thiscall* OriginalFn)(PVOID, matrix3x4*, int, int, float);
		return getvfunc<OriginalFn>(pRenderable, 16)(pRenderable, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	}

	bool& Glowing()
	{
		static int offset = g_Netvar.get_offset("DT_TFPlayer", "m_bGlowEnabled");
		return *(bool*)(this + offset);
	}

	bool Dormant()
	{
		PVOID pNetworkable = (PVOID)(this + 0x8);
		typedef bool(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pNetworkable, 8)(pNetworkable);
	}


	int DrawModel(int flags)
	{
		PVOID pRenderable = (PVOID)(this + 0x4);
		typedef int(__thiscall* OriginalFn)(PVOID, int);
		return getvfunc<OriginalFn>(pRenderable, 10)(pRenderable, flags);
	}

	ClientClass* fClientClass()
	{
		PVOID pNetworkable = (PVOID)(this + 0x8);
		typedef ClientClass* (__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pNetworkable, 2)(pNetworkable);
	}

	int Index()
	{
		PVOID pNetworkable = (PVOID)(this + 0x8);
		typedef int(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pNetworkable, 9)(pNetworkable);
	}

	void RenderBounds(Vector& mins, Vector& maxs)
	{
		PVOID pRenderable = (PVOID)(this + 0x4);
		typedef void(__thiscall* OriginalFn)(PVOID, Vector&, Vector&);
		getvfunc<OriginalFn>(pRenderable, 20)(pRenderable, mins, maxs);
	}

	matrix3x4& RgflCoordinateFrame()
	{
		PVOID pRenderable = static_cast<PVOID>(this + 0x4);
		typedef matrix3x4 &(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pRenderable, 34)(pRenderable);
	}

	int MaxHealth()
	{
		typedef int(__thiscall *OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 107)(this);
	}

	void UpdateGlowEffect()
	{
		typedef void(__thiscall* OriginalFn)(CBaseEntity*);
		return getvfunc<OriginalFn>(this, 226)(this);
	}

	void DestroyGlowEffect()
	{
		typedef void(__thiscall* OriginalFn)(CBaseEntity*);
		return getvfunc<OriginalFn>(this, 227)(this);
	}

	int Health()
	{
		DYNVAR_RETURN(int, this, "DT_BasePlayer", "m_iHealth");
	}

	int TeamNum()
	{
		DYNVAR_RETURN(int, this, "DT_BaseEntity", "m_iTeamNum");
	}

	float& FovRate()
	{
		static int offset = 0xE5C;
		return *(float*)(this + offset);
	}

	int& DefaultFov()
	{
		static int offset = g_Netvar.get_offset("DT_BasePlayer", "m_iDefaultFOV");
		return *(int*)(this + offset);
	}

	int& Fov()
	{
		static int offset = g_Netvar.get_offset("DT_BasePlayer", "m_iFOV");
		return *(int*)(this + offset);
	}

	int& Flags()
	{
		static int offset = g_Netvar.get_offset("DT_BasePlayer", "m_fFlags");
		return *(int*)(this + offset);
	}

	BYTE LifeState()
	{
		DYNVAR_RETURN(BYTE, this, "DT_BasePlayer", "m_lifeState");
	}

	int ClassNum()
	{
		DYNVAR_RETURN(int, this, "DT_TFPlayer", "m_PlayerClass", "m_iClass");
	}

	const char* szClass()
	{
		switch (ClassNum())
		{
		case TF2_Scout:
			return "Scout";
		case TF2_Soldier:
			return "Soldier";
		case TF2_Pyro:
			return "Pyro";
		case TF2_Demoman:
			return "Demoman";
		case TF2_Heavy:
			return "Heavy";
		case TF2_Engineer:
			return "Engineer";
		case TF2_Medic:
			return "Medic";
		case TF2_Sniper:
			return "Sniper";
		case TF2_Spy:
			return "Spy";
		default:
			return "Unknown class";
		}

		return "Unknown class"; //Just in case
	}

	int Cond()
	{
		DYNVAR_RETURN(int, this, "DT_TFPlayer", "m_Shared", "m_nPlayerCond");
	}

	int& CondRef()
	{
		static int offset = g_Netvar.get_offset("DT_TFPlayer", "m_Shared", "m_nPlayerCond");
		return *(int*)(this + offset);
	}

	int& Tickbase()
	{
		static int offset = g_Netvar.get_offset("DT_BasePlayer", "localdata", "m_nTickBase");
		return *(int*)(this + offset);
	}

	float FovTime()
	{
		DYNVAR_RETURN(int, this, "DT_BasePlayer", "m_flFOVTime");
	}

	float NextPrimaryAttack()
	{
		DYNVAR_RETURN(float, this, "DT_BaseCombatWeapon", "LocalActiveWeaponData", "m_flNextPrimaryAttack");
	}

	float LastFireTime()
	{
		DYNVAR_RETURN(float, this, "DT_TFWeaponBase", "LocalActiveTFWeaponData", "m_flLastFireTime");
	}

	CBaseCombatWeapon* ActiveWep()
	{
		DYNVAR(pHandle, DWORD, "DT_BaseCombatCharacter", "m_hActiveWeapon");
		return (CBaseCombatWeapon *)g_Interfaces.EntList->GetClientEntityFromHandle(pHandle.GetValue(this));
	}
};

class CBaseCombatWeapon : public CBaseEntity
{
public:
	int Clip1()
	{
		DYNVAR_RETURN(int, this, "DT_BaseCombatWeapon", "LocalWeaponData", "m_iClip1");
	}

	int GetMaxClip2()
	{
		typedef int(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 319)(this);
	}

	int GetSlot()
	{
		typedef int(__thiscall *OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 327)(this);
	}

	int DefIndex()
	{
		DYNVAR_RETURN(int, this, "DT_EconEntity", "m_AttributeManager", "m_Item", "m_iItemDefinitionIndex");
	}

	char *GetName()
	{
		typedef char *(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 329)(this);
	}

	char *GetPrintName()
	{
		typedef char *(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 330)(this);
	}

	bool CalcIsCrit()
	{
		typedef bool(__thiscall* OriginalFn)(CBaseCombatWeapon*);

		static DWORD dwFunc = g_Sig.Find("client.dll", "55 8B EC 83 EC 18 56 57 6A 00 68 ? ? ? ? 68 ? ? ? ? 6A 00 8B F9 E8 ? ? ? ? 50 E8 ? ? ? ? 8B F0 83 C4 14 89 75 EC");

		return ((OriginalFn)dwFunc)(this);
	}
};