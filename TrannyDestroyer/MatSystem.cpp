#include "MatSystem.h"
#include "KeyValues.h"
#include "Materials.h"
#include "Interfaces.h"

CMat g_Mat;

void CMat::Initialize()
{
	glow = g_Interfaces.MatSystem->FindMaterial("dev/glow_color", "Model textures");
	shaded = CreateMaterial(false, false);
	shiny = CreateMaterial(false, false, false, true);
	wireframe = CreateMaterial(true, true, true);

	hand_glow = g_Interfaces.MatSystem->FindMaterial("dev/glow_color", "Model textures");
	hand_shaded = CreateMaterial(false, false);
	hand_shiny = CreateMaterial(false, false, false, true);
	hand_wireframe = CreateMaterial(false, true, true);

	// Prevent the glow material from being deleted at any point
	glow->IncrementReferenceCount();
}

Matptr CMat::CreateMaterial(bool IgnoreZ, bool Flat, bool Wireframe, bool Shiny)
{
	static int created = 0;

	static const char tmp[] =
	{
		"\"%s\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$envmap\" \"%s\"\
		\n\t\"$normalmapalphaenvmapmask\" \"%i\"\
		\n\t\"$envmapcontrast\" \"%i\"\
		\n\t\"$model\" \"1\"\
		\n\t\"$flat\" \"1\"\
		\n\t\"$nocull\" \"0\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$ignorez\" \"1\"\
		\n\t\"$halflambert\" \"1\"\
		\n\t\"$nofog\" \"0\"\
		\n\t\"$ignorez\" \"%i\"\
		\n\t\"$znearer\" \"0\"\
		\n\t\"$wireframe\" \"%i\"\
        \n}\n"
	};

	const char* baseType = (Flat ? "UnlitGeneric" : "VertexLitGeneric");
	char material[512];
	sprintf_s(material, sizeof(material), tmp, baseType, Shiny ? "env_cubemap" : "", Shiny ? 1 : 0, Shiny ? 1 : 0, (IgnoreZ) ? 1 : 0, (Wireframe) ? 1 : 0);

	char name[512];
	sprintf_s(name, sizeof(name), "#trannydestroyer_mat_%i.vmt", created);
	created++;

	KeyValues* keyValues = (KeyValues*)malloc(sizeof(KeyValues));
	KeyValues::Initialize(keyValues, strdup(baseType));
	KeyValues::LoadFromBuffer(keyValues, name, material);

	IMaterial *createdMaterial = g_Interfaces.MatSystem->CreateMaterial(name, keyValues);
	if (!createdMaterial)
		return nullptr;

	createdMaterial->IncrementReferenceCount();

	return createdMaterial;
}

void CMat::ForceMaterial(Color color, Matptr material, bool useColor, bool forceMaterial)
{
	if (useColor)
	{
		float blend[3] = { (float)color[0] / 255.f, (float)color[1] / 255.f, (float)color[2] / 255.f };
		float alpha = (float)color[3] / 255.f;

		g_Interfaces.RenderView->SetBlend(alpha);
		g_Interfaces.RenderView->SetColorModulation(blend);
	}
	 
	if (forceMaterial)
		g_Interfaces.ModelRender->ForcedMaterialOverride(material);
}

void CMat::ResetMaterial()
{
	static const float flDefault[3] = { 1, 1, 1 };
	g_Interfaces.RenderView->SetBlend(1);
	g_Interfaces.RenderView->SetColorModulation(flDefault);
	g_Interfaces.ModelRender->ForcedMaterialOverride(nullptr);
}