#include "Interfaces.h"
#include "Signature.h"
#include "Base.h"
#include "Menu.h"

Signature g_Sig;
Interfaces g_Interfaces;

__forceinline CreateInterfaceFn CreateFactory(const char* modname)
{
	return (CreateInterfaceFn)GetProcAddress(GetModuleHandle(modname), "CreateInterface");
}

template <class C>
inline C* CreateInterface(const char* module, const char* intName)
{
	CreateInterface_t intF = CreateFactory(module);

	return (C*)intF(intName, NULL);
}

void Interfaces::Init()
{
	Cvar = CreateInterface<ICvar>("vstdlib.dll", XorStr("VEngineCvar004"));

	CONOUT("Client DLL Loaded!");

	Trace     =    CreateInterface<IEngineTrace>("engine.dll", XorStr("EngineTraceClient003"));
	ModelInfo =    CreateInterface<IModelInfo>("engine.dll", XorStr("VModelInfoClient006"));
	Surface   =    CreateInterface<ISurface>("vguimatsurface.dll", XorStr("VGUI_Surface030"));
	EntList   =    CreateInterface<IEntList>("client.dll", XorStr("VClientEntityList003"));
	Engine    =    CreateInterface<IEngine>("engine.dll", XorStr("VEngineClient013"));
	Panels    =    CreateInterface<IPanel>("vgui2.dll", XorStr("VGUI_Panel009"));
	Client    =    CreateInterface<IClient>("client.dll", XorStr("VClient017"));
	GameMovement = CreateInterface<CGameMovement>("client.dll", XorStr("GameMovement001"));
	Pred =		   CreateInterface<CPrediction>("client.dll", XorStr("VClientPrediction001"));
	ModelRender =  CreateInterface<CModelRender>("engine.dll", XorStr("VEngineModel016"));
	RenderView =   CreateInterface<CRenderView>("engine.dll", XorStr("VEngineRenderView014"));
	MatSystem =    CreateInterface<CMaterialSystem>("MaterialSystem.dll", XorStr("VMaterialSystem081"));

	ClientMode =  **(ClientModeShared***)g_Sig.Find("client.dll", XorStr("8B 0D ? ? ? ? 8B 02 D9 05"), 0x2);
	ClientState = *(CClientState**)(g_Sig.Find("engine.dll", XorStr("B9 ? ? ? ? 50 E8 ? ? ? ? 5E 8B E5 5D C3"), 0x1));
	Globals = **(IGlobals***)g_Sig.Find("client.dll", XorStr("A1 ? ? ? ? C1 E7 03"), 0x1);
	Input = **(CInput***)(g_Sig.Find("client.dll", "8B 0D ? ? ? ? 56 8B 01 FF 50 24 8B 45 FC", 0x2));
	g_PredictionRandomSeed = *(int**)g_Sig.Find("client.dll", XorStr("C7 05 ? ? ? ? ? ? ? ? 5D C3 8B 40 34"), 0x2);

	CHKPTR(g_PredictionRandomSeed);
	CHKPTR(Pred);
	CHKPTR(Input);
	CHKPTR(ClientMode);
	CHKPTR(ClientState);

	HWND thisWindow;
	while (!(thisWindow = FindWindow("Valve001", NULL)))
		Sleep(500);
	g_Menu.windowProc = (WNDPROC)SetWindowLongPtr(thisWindow, GWLP_WNDPROC, (LONG_PTR)&Hooked_WndProc);
	CONOUT("Finished grabbing pointers.");
}