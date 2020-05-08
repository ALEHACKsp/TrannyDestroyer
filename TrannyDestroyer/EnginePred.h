#pragma once
#include "Prediction.h"

class CPredictionSystem
{
public:
	void PredictNextTick(CUserCmd* cmd);

	bool bPredicting = false;
private:
	float m_flOldCurtime;
	float m_flOldFrametime;
	int* m_pPredictionRandomSeed;
};

extern CPredictionSystem g_Pred;