#pragma once
#include "VMT.h"
#include <Windows.h>

class CUserCmd;

struct DrawModelState_t;
struct ModelRenderInfo_t;
class CModelRender;
typedef float matrix3x4[3][4];
typedef void(__thiscall *DrawModelExecuteFn)(CModelRender*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4*);

enum ClientFrameStage_t
{
	FRAME_UNDEFINED = -1,
	FRAME_START,

	// A network packet is being recieved
	FRAME_NET_UPDATE_START,

	// Data has been received and we're going to start calling PostDataUpdate
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,

	// Data has been received and we've called PostDataUpdate on all data recipients
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,

	// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_NET_UPDATE_END,

	// We're about to start rendering the scene
	FRAME_RENDER_START,

	// We've finished rendering the scene.
	FRAME_RENDER_END
};

class Hooks
{
public:
	void Init();

	HMODULE CheatModule;

	VMTBaseManager* ClientModeHook;
	VMTBaseManager* ClientHook;
	VMTBaseManager* PanelHook;
	VMTBaseManager* PredictionHook;
	VMTBaseManager* ModelRenderHook;
	VMTBaseManager* RenderViewHook;
	VMTBaseManager* InputHook;
};

extern Hooks g_Hooks;

bool __fastcall Hooked_CreateMove(void* ClientMode, int edx, float input_sample_frametime, CUserCmd* pCommand);
void __fastcall Hooked_PaintTraverse(void* pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce);