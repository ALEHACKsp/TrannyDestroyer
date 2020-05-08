#include "Resolver.h"
#include "Interfaces.h"
#include "Engine.h"
#include "Base.h"
#include "Entities.h"
#include <unordered_map>
#include "Aimbot.h"
Resolver g_Resolver;

std::unordered_map<int, Vector> SavedAngles;

void Resolver::Frame()
{
	for (int i = 0; i <= g_Interfaces.Engine->GetMaxClients(); i++)
	{
		if (i == me->Index())
			continue;

		CBaseEntity* target = GetEnt(i);

		if (!target)
			continue;

		if (target->Dormant())
			continue;

		if (target->TeamNum() == me->TeamNum())
			continue;

		if (g_Aimbot.last_target != target->Index())
			continue;

		Vector& EyeAngles = target->EyeAngles();

		switch (shots)
		{
		case 0: EyeAngles.x = 89; EyeAngles.y -= 45; break;
		case 1: EyeAngles.x = -89; EyeAngles.y -= 45; break;
		case 2: EyeAngles.x = 89; EyeAngles.y -= 45; break;
		case 3: EyeAngles.x = -89; EyeAngles.y -= 45; break;
		case 4: EyeAngles.x = 89; EyeAngles.y -= 45; break;
		case 5: EyeAngles.x = -89; EyeAngles.y -= 45; break;
		case 6: shots = 0; EyeAngles.x = 89; EyeAngles.y -= 45; break;
		}
	}
}