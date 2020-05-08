#include "Misc.h"
#include "Engine.h"
#include "Base.h"
#include "Interfaces.h"
#include "Entities.h"

Misc g_Misc;

void Misc::Tick(CUserCmd* cmd)
{
	if (!(me->Flags() & FL_ONGROUND) && cmd->buttons & IN_JUMP)
	{
		//Autostrafe	
		if (autostrafe.value)
			if (cmd->mousedx > 1 || cmd->mousedx < -1)  //> 1 < -1 so we have some wiggle room
				cmd->sidemove = cmd->mousedx > 1 ? 450.f : -450.f;

		//Bunnyhop
		if (bhop.value)
			cmd->buttons &= ~IN_JUMP;
	}
}