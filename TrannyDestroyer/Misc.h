#pragma once
#include "Controls.h"

class CUserCmd;
class Misc
{
public:
	void Tick(CUserCmd* cmd);

	Checkbox bhop = Checkbox("Bhop");
	Checkbox autostrafe = Checkbox("Autostrafe");
	Checkbox enginepred = Checkbox("Enable Engine Prediction");

	Checkbox unload = Checkbox("Unload");
};

extern Misc g_Misc;