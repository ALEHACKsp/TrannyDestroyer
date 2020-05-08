#include "Hooks.h"
#include "Entities.h"
#include "Engine.h"
#include "Interfaces.h"
#include "VGUI.h"
#include "Menu.h"
#include "Esp.h"
#include "Aimbot.h"
#include "Misc.h"
#include "Triggerbot.h"
#include "EnginePred.h"
#include "Antiaim.h"
#include "MatSystem.h"
#include "Materials.h"
#include "Resolver.h"
#include "Crits.h"
#include "Input.h"
#include "MD5.h"
#include "ClientState.h"

Hooks g_Hooks;

//====================================================================================================================
void SilentFix(CUserCmd *Cmd, Vector vOldAng)
{
	Vector vecSilent(Cmd->forwardmove, Cmd->sidemove, Cmd->upmove), angMove;
	float flSpeed = vecSilent.Length2D();
	VectorAngles(vecSilent, angMove);
	float flYaw = DEG2RAD(Cmd->viewangles.y - vOldAng.y + angMove.y);
	Cmd->forwardmove = cos(flYaw) * flSpeed, Cmd->sidemove = sin(flYaw) * flSpeed;
}
//====================================================================================================================
bool __fastcall Hooked_CreateMove(void* ClientMode, int edx, float input_sample_frametime, CUserCmd * pCommand)
{
	try
	{
		VMTManager& hook = VMTManager::GetHook(ClientMode);
		bool bReturn = hook.GetMethod<bool(__thiscall*)(void*, float, CUserCmd*)>((int)e_offset::CreateMove)(ClientMode, input_sample_frametime, pCommand);

		if (pCommand->command_number == 0)
			return bReturn;

		if (!me)
			return bReturn;

		if (me->LifeState() != LIFE_ALIVE)
			return bReturn;

		if (!me->ActiveWep())
			return bReturn;

		//if (pCommand->buttons & IN_ATTACK && g_Aimbot.CanShoot)
		//	g_Resolver.shots++;

		g_Aimbot.CalcCanShoot(me);

		Vector vOld = pCommand->viewangles;

		void *createMoveEBP = NULL;
		__asm mov createMoveEBP, ebp;
		bool* bSendPacket = (bool*)(***(byte****)createMoveEBP) - 0x1;

		g_Crits.Tick(pCommand);

		if (g_Misc.enginepred.value && g_Crits.bShouldEnginePredict)
			g_Pred.PredictNextTick(pCommand);

		g_AntiAim.Tick(pCommand, bSendPacket);

		g_Aimbot.Tick(me, pCommand);

		g_Misc.Tick(pCommand);

		g_Triggerbot.Tick(pCommand);

		SilentFix(pCommand, vOld);

		return false;
	}
	catch(...)
	{
		//MessageBoxA(NULL, "TrannyDestroyer", "CreateMove failure", MB_OK);
	}
	return false;
}
//====================================================================================================================
void __fastcall Hooked_PaintTraverse(void * pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	try
	{
		VMTManager& hook = VMTManager::GetHook(pPanels); //Get a pointer to the instance of your VMTManager with the function GetHook.
		static Panel FOP = NULL;
		static Panel HudScope = NULL;

		if (!FOP)
		{
			const char* name = g_Interfaces.Panels->GetName(vguiPanel);
			if (!strcmp(name, XorStr("FocusOverlayPanel")))
			{
				FOP = vguiPanel;

				g_Draw.Init();
				InitTextures();
				g_Netvar.Initialize();
				g_Mat.Initialize();
				g_Menu.CreateGUI();
			}
		}

		if (!HudScope)
		{
			const char* name = g_Interfaces.Panels->GetName(vguiPanel);

			if (!strcmp(name, XorStr("HudScope")))
			{
				HudScope = vguiPanel;
			}
		}

		if (g_Esp.misc_noscope.value && vguiPanel == HudScope)
			return;

		hook.GetMethod<void(__thiscall*)(void*, unsigned int, bool, bool)>((int)e_offset::PaintTraverse)(pPanels, vguiPanel, forceRepaint, allowForce); 

		if (FOP == vguiPanel)
		{
			g_Interfaces.Panels->SetTopmostPopup(FOP, g_Menu.enabled);

			CScreenSize newSize;
			g_Interfaces.Engine->GetScreenSize(newSize.width, newSize.height);
			if (newSize.width != gScreenSize.width || newSize.height != gScreenSize.height || (GetAsyncKeyState(VK_F4) & 1))
				g_Draw.Reload(); // Reload stuff like fonts and screen size to prevent resolution change/fullscreen issues

			g_Menu.GetInput();

			g_Esp.Players();

			g_Menu.Draw();

			g_Crits.Frame();

			g_Menu.EndInput();

			if (!g_Misc.enginepred.value && !g_Hooks.PredictionHook->Hooked())
			{
				g_Hooks.PredictionHook->Rehook();
			}

			g_Interfaces.Panels->SetMouseInputEnabled(vguiPanel, g_Menu.enabled);
		}
	}
	catch (...)
	{
		MessageBoxA(NULL, "TrannyDestroyer", "PaintTraverse failure", MB_OK);
	}
}
//====================================================================================================================
void __fastcall Hooked_RunCommand(PVOID Prediction, int edx, CBaseEntity *pBaseEntity, CUserCmd *pCommand, IMoveHelper *MoveHelper)
{
	if (MoveHelper != nullptr)
		g_Interfaces.MoveHelper = MoveHelper;

	VMTManager& hook = VMTManager::GetHook(g_Interfaces.Pred);

	if (g_Interfaces.MoveHelper != nullptr && me && g_Misc.enginepred.value && g_Interfaces.Engine->IsInGame() && me->LifeState() == LIFE_ALIVE)
		hook.Unhook();

	return hook.GetMethod<void(__thiscall*)(void*, CBaseEntity*, CUserCmd*, IMoveHelper*)>((int)e_offset::RunCommand)(Prediction, pBaseEntity, pCommand, MoveHelper); //Call the original.
}
//====================================================================================================================
void __stdcall Hooked_FrameStageNotify(ClientFrameStage_t stage)
{
	VMTManager & hook = VMTManager::GetHook(g_Interfaces.Client);
	if (!me)
		return hook.GetMethod<void(__thiscall *)(void*, ClientFrameStage_t)>((int)e_offset::FrameStageNotify)(g_Interfaces.Client, stage);

	//if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START && me && me->LifeState() == LIFE_ALIVE)
		//g_Resolver.Frame();

	if (stage == FRAME_RENDER_START && g_Esp.misc_nozoom.value)
	{
		me->Fov() = me->DefaultFov();
		me->FovRate() = 0;
	}

	hook.GetMethod<void(__thiscall *)(void*, ClientFrameStage_t)>((int)e_offset::FrameStageNotify)(g_Interfaces.Client, stage);
}
//====================================================================================================================
#define RET return CallOriginal(g_Interfaces.ModelRender, state, pInfo, pCustomBoneToWorld)
void __stdcall Hooked_DrawModelExecute(const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld)
{
	VMTManager& hook = VMTManager::GetHook(g_Interfaces.ModelRender);
	auto CallOriginal = hook.GetMethod<DrawModelExecuteFn>((int)e_offset::DrawModelExecute);

	if (!pInfo.pModel)
		return CallOriginal(g_Interfaces.ModelRender, state, pInfo, pCustomBoneToWorld);


	hook.Unhook();
	if (g_Esp.chams_enabled.value)
	{
		if(!g_Esp.Chams(GetEnt(pInfo.entity_index), CallOriginal, state, pInfo, pCustomBoneToWorld))
		{
			hook.Rehook();
			return;
		}
	}
	hook.Rehook();

	CallOriginal(g_Interfaces.ModelRender, state, pInfo, pCustomBoneToWorld);
	g_Mat.ResetMaterial();
}
//====================================================================================================================
bool __fastcall Hooked_ShouldDrawViewModel(void* ClientMode)
{
	if (g_Esp.misc_nozoom.value && g_Esp.misc_drawvmodel.value)
	{
		return true;
	}
	else
	{
		VMTManager & hook = VMTManager::GetHook(ClientMode);
		return hook.GetMethod<bool(__thiscall*)(void*)>((int)e_offset::ShouldDrawViewModel)(ClientMode);
	}
}
//====================================================================================================================
void __fastcall SceneEnd(void* RenderView)
{
	VMTManager & hook = VMTManager::GetHook(RenderView);
	hook.GetMethod<void(__thiscall *)(void*)>(9)(RenderView);
}
//====================================================================================================================
CUserCmd* __stdcall Hooked_GetUserCmd(int sequence_number)
{
	auto& hook = VMTManager::GetHook(g_Interfaces.Input);
	CUserCmd* pCommand = hook.GetMethod<CUserCmd*(__thiscall*)(void*, int)>(8)(g_Interfaces.Input, sequence_number);

	if (pCommand == nullptr)
		return pCommand;

	int oldcmd;
	if (g_Crits.new_command_number.find(pCommand->command_number) != g_Crits.new_command_number.end())
	{
		oldcmd = pCommand->command_number;

		pCommand->command_number = g_Crits.new_command_number[pCommand->command_number];

		pCommand->random_seed = MD5_PseudoRandom(unsigned(pCommand->command_number)) & 0x7fffffff;

		g_Crits.new_command_number.erase(g_Crits.new_command_number.find(oldcmd));

		g_Interfaces.ClientState->lastoutgoingcommand = pCommand->command_number - 1;

		if (g_Interfaces.Engine->GetNetChannelInfo())
			g_Interfaces.Engine->GetNetChannelInfo()->m_nOutSequenceNr = pCommand->command_number - 1;
	}

	return pCommand;
}
//====================================================================================================================
void Hooks::Init()
{
	ClientModeHook = new VMTBaseManager(g_Interfaces.ClientMode);
	ClientModeHook->HookMethod(&Hooked_CreateMove, e_offset::CreateMove);
	ClientModeHook->HookMethod(&Hooked_ShouldDrawViewModel, e_offset::ShouldDrawViewModel);
	ClientModeHook->Rehook();

	ClientHook = new VMTBaseManager(g_Interfaces.Client);
	ClientHook->HookMethod(&Hooked_FrameStageNotify, e_offset::FrameStageNotify);
	ClientHook->Rehook();

	InputHook = new VMTBaseManager(g_Interfaces.Input);
	InputHook->HookMethod(&Hooked_GetUserCmd, 8);
	InputHook->Rehook();

	PanelHook = new VMTBaseManager(g_Interfaces.Panels);
	PanelHook->HookMethod(&Hooked_PaintTraverse, e_offset::PaintTraverse);
	PanelHook->Rehook();

	PredictionHook = new VMTBaseManager(g_Interfaces.Pred);
	PredictionHook->HookMethod(&Hooked_RunCommand, e_offset::RunCommand);
	PredictionHook->Rehook();

	ModelRenderHook = new VMTBaseManager(g_Interfaces.ModelRender);
	ModelRenderHook->HookMethod(&Hooked_DrawModelExecute, e_offset::DrawModelExecute);
	ModelRenderHook->Rehook();
}