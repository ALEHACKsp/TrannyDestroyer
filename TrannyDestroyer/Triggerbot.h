#pragma once
#include "Controls.h"
class CUserCmd;

class Triggerbot
{
public:
	void Tick(CUserCmd* cmd);

	Checkbox master = Checkbox("Enabled");
	Listbox hitbox = Listbox("Hitbox", {"Head", "Body", "All"});
	Checkbox scoped = Checkbox("Scoped only");
	Checkbox ignore_cloak = Checkbox("Ignore Cloak", true);
	Checkbox ignore_disguise = Checkbox("Ignore Disguise", true);
	KeyBind trigkey = KeyBind("Key", 'Q');
};

extern Triggerbot g_Triggerbot;