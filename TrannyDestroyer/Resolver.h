#pragma once
#include "Controls.h"

class Resolver
{
public:
	void Frame();

	int shots;

	Checkbox resolver_enabled = Checkbox("Enabled");
private:
	float LastYaw = 0.0f;
};

extern Resolver g_Resolver;