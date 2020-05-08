#pragma once

class IEngine;
class IClient;
class IEntList;
class IEngineTrace;
class IGlobals;
class IModelInfo;
class CGameMovement;
class CPrediction;
class ISurface;
class IPanel;
class IMoveHelper;
class CInput;
class CClientState;
class CModelRender;
class CRenderView;
class CMaterialSystem;

class ClientModeShared;

class ICvar;

class Interfaces
{
public:
	void Init();

	IEngine* Engine;
	IClient* Client;
	IEntList* EntList;
	IModelInfo* ModelInfo;
	IEngineTrace* Trace;
	IGlobals* Globals;
	CGameMovement* GameMovement;
	CPrediction* Pred;
	IMoveHelper* MoveHelper;
	ICvar* Cvar;
	CModelRender* ModelRender;
	CRenderView* RenderView;
	CMaterialSystem* MatSystem;
	CInput* Input;
	ClientModeShared* ClientMode;
	CClientState* ClientState;
	ISurface* Surface;
	IPanel* Panels;

	int* g_PredictionRandomSeed = nullptr;
};

extern Interfaces g_Interfaces;