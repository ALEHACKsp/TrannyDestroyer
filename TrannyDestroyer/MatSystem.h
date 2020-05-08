#pragma once
#include "Draw utils.h"

class IMaterial;
typedef IMaterial* Matptr;

class CMat
{
public:
	Matptr shaded, shiny, glow, wireframe;
	Matptr hand_shaded, hand_shiny, hand_glow, hand_wireframe;
	void Initialize();

	Matptr CreateMaterial(bool IgnoreZ, bool Flat, bool Wireframe = false, bool Shiny = false);
	void ForceMaterial(Color Color, Matptr Material, bool ForceColor = true, bool ForceMat = true);
	void ResetMaterial();
};
extern CMat g_Mat;