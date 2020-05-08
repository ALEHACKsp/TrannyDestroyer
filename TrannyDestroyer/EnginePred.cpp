#include "EnginePred.h"
#include "Signature.h"
#include "Interfaces.h"
#include "Base.h"
#include "MD5.h"

CPredictionSystem g_Pred;

void CPredictionSystem::PredictNextTick(CUserCmd* cmd)
{
	bPredicting = true;
	// back up the globals
	float   oldCurTime = g_Interfaces.Globals->curtime;
	float   oldFrameTime = g_Interfaces.Globals->frametime;
	int     oldTickcount = g_Interfaces.Globals->tickcount;
	int     oldflags = me->Flags();
	int     oldbuttons = g_Interfaces.GameMovement->mv->m_nButtons;
	int		oldtickbase = me->Tickbase();

	// set up the globals
	g_Interfaces.Globals->tickcount = me->Tickbase();
	g_Interfaces.Globals->curtime = me->Tickbase() * g_Interfaces.Globals->interval_per_tick;
	g_Interfaces.Globals->frametime = g_Interfaces.Globals->interval_per_tick;

	
	*g_Interfaces.g_PredictionRandomSeed = MD5_PseudoRandom(cmd->command_number) & 0x7FFFFFFF;
	g_Interfaces.Pred->RunCommand(me, cmd, g_Interfaces.MoveHelper);


	// restore
	g_Interfaces.Globals->curtime = oldCurTime;
	g_Interfaces.Globals->frametime = oldFrameTime;
	g_Interfaces.Globals->tickcount = oldTickcount;
	me->Flags() = oldflags;
	g_Interfaces.GameMovement->mv->m_nButtons = oldbuttons;

	if(g_Interfaces.Globals->frametime > 0) // are we sane?
		me->Tickbase() = oldtickbase;

	bPredicting = false;
}