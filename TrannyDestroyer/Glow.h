//#pragma once
//class Color;
//#include "Entities.h"
//
//class KeyValues;
//
//class IScreenSpaceEffect
//{
//public:
//	virtual void Init() = 0;
//	virtual void Shutdown() = 0;
//
//	virtual void SetParameters(KeyValues *params) = 0;
//
//	virtual void Render(int x, int y, int w, int h) = 0;
//
//	virtual void Enable(bool bEnable) = 0;
//	virtual bool IsEnabled() = 0;
//};
//
//class CScreenSpaceEffectRegistration
//{
//public:
//	CScreenSpaceEffectRegistration(const char *pName,
//		IScreenSpaceEffect *pEffect);
//
//	const char *m_pEffectName;
//	IScreenSpaceEffect *m_pEffect;
//
//	CScreenSpaceEffectRegistration *m_pNext;
//
//	// NEVER USE - CATHOOK
//	static CScreenSpaceEffectRegistration *s_pHead;
//};
//
//class EffectGlow : public IScreenSpaceEffect
//{
//public:
//	virtual void Init();
//	inline virtual void Shutdown()
//	{
//		mat_unlit.Shutdown();
//		mat_unlit_z.Shutdown();
//		mat_blit.Shutdown();
//		mat_unlit.Shutdown();
//		mat_unlit_z.Shutdown();
//		mat_blur_x.Shutdown();
//		mat_blur_y.Shutdown();
//	}
//
//	virtual void Render(int x, int y, int w, int h);
//
//	inline virtual void Enable(bool bEnable)
//	{
//		enabled = bEnable;
//	}
//	inline virtual bool IsEnabled()
//	{
//		return enabled;
//	}
//
//	void StartStenciling();
//	void EndStenciling();
//	void DrawEntity(CBaseEntity *entity);
//	void DrawToStencil(CBaseEntity *entity);
//	void DrawToBuffer(CBaseEntity *entity);
//	Color GlowColor(CBaseEntity *entity);
//	bool ShouldRenderGlow(CBaseEntity *entity);
//	void RenderGlow(CBaseEntity *entity);
//	void BeginRenderGlow();
//	void EndRenderGlow();
//
//public:
//	bool init{ false };
//	bool drawing{ false };
//	bool enabled;
//	float orig_modulation[3];
//	CMaterialReference mat_blit;
//	CMaterialReference mat_blur_x;
//	CMaterialReference mat_blur_y;
//	CMaterialReference mat_unlit;
//	CMaterialReference mat_unlit_z;
//};
//
//extern EffectGlow g_EffectGlow;