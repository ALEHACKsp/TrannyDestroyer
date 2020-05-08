#include "Triggerbot.h"
#include "Entities.h"
#include "Engine.h"
#include "Interfaces.h"
#include "Base.h"
#include "Trace.h"

Triggerbot g_Triggerbot;

void Triggerbot::Tick(CUserCmd * cmd)
{
	if (!trigkey.KeyDown())
		return;

	if (!master.value)
		return;

	if (!me->ActiveWep())
		return;

	int index = me->ActiveWep()->DefIndex();
	if (scoped.value && me->ClassNum() == TF2_Sniper && !(me->Cond() & TFCond_Zoomed) && (index == WPN_SniperRifle ||
		index == WPN_Machina || index == WPN_SydneySleeper || index == WPN_FestiveSniperRifle || index == WPN_AWP))
		return;

	Vector vAim;
	g_Interfaces.Engine->GetViewAngles(vAim);

	Vector vForward;
	Vector eyePos = me->EyePos();

	AngleVectors(vAim, &vForward);
	vForward = vForward * 9999 + me->EyePos();

	Ray_t ray;
	trace_t trace;
	CTraceFilterCustom filt(me);
	filt.pSkip = me;

	ray.Init(eyePos, vForward);

	g_Interfaces.Trace->TraceRay(ray, 0x46004003, &filt, &trace);

	if (!trace.m_pEnt)
		return;

	if (trace.hitgroup < 1)
		return;

	if (trace.m_pEnt->TeamNum() == me->TeamNum())
		return;

	if (trace.m_pEnt->Cond() & TFCond_Ubercharged ||
		trace.m_pEnt->Cond() & TFCond_UberchargeFading ||
		trace.m_pEnt->Cond() & TFCond_Bonked)
		return;

	switch (hitbox.value)
	{
	case 0:
		if (trace.hitbox)
			return;
	case 1:
		if (trace.hitbox <= 5)
			return;
	}

	cmd->buttons |= IN_ATTACK;
}
