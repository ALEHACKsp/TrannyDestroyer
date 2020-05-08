#pragma once
#include "Controls.h"

class CUserCmd;
class AntiAim
{
public:
	void Tick(CUserCmd* cmd, bool* bSendPacket);

	Checkbox master = Checkbox("Enabled", false);

	Checkbox pitch = Checkbox("Pitch");
	Checkbox pitch_bsendpacket = Checkbox("Pitch Jitter bSendPacket");
	Listbox real_pitch = Listbox("Real Pitch", { "Up", "Half up", "Center", "Down" });
	Listbox fake_pitch = Listbox("Fake Pitch", { "Down", "Up" });


	Checkbox yaw = Checkbox("Yaw");
	Checkbox yaw_bsendpacket = Checkbox("Use bSendPacket");

	Listbox real_yaw = Listbox("Real Yaw", {"None", "Current Y", "Left", "Right"});
	Listbox fake_yaw = Listbox("Fake Yaw", {"None", "Left", "Right", "Current Y", "Edge"});
};

extern AntiAim g_AntiAim;