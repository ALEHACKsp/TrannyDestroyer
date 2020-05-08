#pragma once
#include "Vector.h"
#include "Trace.h"

class Hit
{
public:
	bool CanHit(void* target, Vector vStart, Vector vEnd);
	bool AssumeVis(CBaseEntity * you, Vector vStart, Vector vEnd, trace_t * result);

};

extern Hit g_Hit;