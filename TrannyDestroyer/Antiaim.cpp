#include "Antiaim.h"
#include "Entities.h"
#include "Engine.h"
#include "Base.h"
#include "Aimbot.h"
#include "Trace.h"
#include "classid.h"
#include "Hit.h"

AntiAim g_AntiAim;

inline float CalcEdge()
{
	trace_t result;
	Vector eyePos = me->EyePos();
	float leastDist = -1, edgeYaw = -1;
	for (int i = 0; i < 360; i += 2)
	{
		Vector angle(0, i, 0), direction;
		AngleVectors(angle, &direction);

		if (g_Hit.AssumeVis(me, eyePos, direction * 90 + eyePos, &result))
			continue;

		Vector normal = result.plane.normal;
		if (abs(normal.y - direction.y) <= 2 && normal.x < 60 && normal.x > -60)
		{
			if (leastDist == -1 || Vector(result.endpos - eyePos).Length() < leastDist)
			{
				edgeYaw = i + 90, leastDist = Vector(result.endpos - eyePos).Length();

				Vector centerEndpos = result.endpos, rightNrm;
				AngleVectors(Vector(0, i - 90, 0), &rightNrm);

				Vector start = rightNrm * 60 + centerEndpos;
				g_Hit.AssumeVis(me, start, start + direction * 80, &result);

				if (result.DidHitWorld())
					edgeYaw = i - 90;
			}
		}
	}

	if (leastDist != -1)
		return edgeYaw;
	else
		return -1;
}

void AntiAim::Tick(CUserCmd * cmd, bool* bSendPacket)
{

	if ((g_Aimbot.target_index && g_Aimbot.master.value && !(g_Aimbot.silent.value)) || ((cmd->buttons & IN_ATTACK) || (cmd->buttons & IN_ATTACK2)))
		if (g_Aimbot.CanShoot)
			return;



}
