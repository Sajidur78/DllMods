#include "pch.h"
#include "Mod.h"
#include "LWDebugMode.h"

app::Mod ms_Mod;

extern "C" __declspec(dllexport) void OnFrame()
{
	ms_Mod.OnFrame();
}

extern "C" __declspec(dllexport) void Init()
{	
	char pathBuf[MAX_PATH];
	GetCurrentDirectoryA(sizeof(pathBuf), pathBuf);

	ms_Mod.Initialize(pathBuf);
	ms_Mod.ApplyPatches();
}