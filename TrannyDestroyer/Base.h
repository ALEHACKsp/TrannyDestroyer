#pragma once
#include <Windows.h>
#include <string>
#include "XorString.h"
#include "ICvar.h"

using namespace std;

typedef void* (__cdecl* CreateInterface_t)(const char*, int*);
typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);

#define CHKPTR(x) if(x == nullptr) MessageBoxA(NULL, XorStr("nullptr"), XorStr("Failed to get ptr"), NULL);

#define GetEnt(x) g_Interfaces.EntList->GetClientEntity(x)
#define me GetEnt(g_Interfaces.Engine->GetLocalPlayer())

#define CONOUT(x) g_Interfaces.Cvar->ConsolePrintf(string("[TrannyDestroyer] " + (string)x + (string)"\n").c_str())

#define WIN32_LEAN_AND_MEAN
#pragma optimize("gsy",on)
#pragma warning( disable : 4244 ) //Possible loss of data
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

class Base
{
public:
};

__forceinline DWORD RESOLVE_CALLGATE(DWORD dwLocation)
{
	return ((*(PDWORD)(dwLocation)) + dwLocation + 4);
}

inline wstring ToWchar(const char* text)
{
	size_t length = strlen(text) + 1;
	wstring wc(length, L'#');
	mbstowcs(&wc[0], text, length);

	return wc;
}

inline string ToChar(const wchar_t* text)
{
	size_t length = wcslen(text) + 1;
	string str(length, L'#');
	wcstombs(&str[0], text, length);

	return str;
}

inline string FloatStr(float input)
{
	string text = to_string(input);
	int last0 = text.find_last_not_of('0'), dot = text.find_first_of('.');

	return text.substr(0, (last0 != string::npos && last0 > dot) ? last0 + 1 : dot);
}


extern Base g_Base;