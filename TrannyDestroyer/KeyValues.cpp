#include "KeyValues.h"
#include "Signature.h"

bool KeyValues::LoadFromBuffer(KeyValues* kv, char const *resourceName, const char *pBuffer, IBaseFileSystem* pFileSystem, const char *pPathID)
{
	static DWORD offset = g_Sig.Find("engine.dll", "55 8B EC 83 EC 38 53 8B 5D 0C");
	typedef int(__thiscall *LoadFromBufferFn)(KeyValues* kv, char const*, const char*, IBaseFileSystem*, const char*);
	static LoadFromBufferFn Load = (LoadFromBufferFn)offset;

	return Load(kv, resourceName, pBuffer, pFileSystem, pPathID);
}

KeyValues* KeyValues::Initialize(KeyValues* kv, char* name)
{
	static DWORD offset = g_Sig.Find("engine.dll", "FF 15 ? ? ? ? 83 C4 08 89 06 8B C6") - 0x42;

	typedef KeyValues*(__thiscall *InitializeFn)(KeyValues*, char*);
	static InitializeFn Init = (InitializeFn)offset;

	return Init(kv, name);
}