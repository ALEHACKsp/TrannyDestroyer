#include "Hit.h"

Hit g_Hit;

bool Hit::CanHit(void * target, Vector vStart, Vector vEnd)
{
	trace_t Trace;
	Ray_t Ray;
	CTraceFilterCustom Filter(me);

	Ray.Init(vStart, vEnd);

	g_Interfaces.Trace->TraceRay(Ray, MASK_SHOT, &Filter, &Trace);

	return Trace.m_pEnt == target;
}

bool Hit::AssumeVis(CBaseEntity* you, Vector vStart, Vector vEnd, trace_t *result)
{
	trace_t Trace;
	Ray_t Ray;
	CTraceFilterCustom Filter(you, nullptr, true);

	Ray.Init(*(Vector*)&vStart, *(Vector*)&vEnd);
	g_Interfaces.Trace->TraceRay(Ray, MASK_SHOT, &Filter, &Trace);

	if (result)
		*result = Trace;

	return !Trace.DidHit();
}

bool CTraceFilterCustom::ShouldHitEntity(CBaseEntity* pEntityHandle, int contentsMask)
{
	CBaseEntity* pEntity = (CBaseEntity *)pEntityHandle;
	ent_id type = (ent_id)pEntity->fClientClass()->iClassID;

	if (predVis && type == ent_id::CTFPlayer)
		return false;

	switch (type)
	{
	case ent_id::CFuncAreaPortalWindow:
	case ent_id::CFuncRespawnRoomVisualizer:
	case ent_id::CSniperDot:
	case ent_id::CTFMedigunShield:
		return false;
	}

	bool shouldHit = true;
	if (you)
		shouldHit = pEntity != you;
	if (shouldHit && pSkip)
		shouldHit = pEntity != pSkip;
	return shouldHit;
}