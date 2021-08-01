#pragma once

#ifndef LW_DEBUG_MODE_DLLL
#define LW_DEBUG_MODE_DLL "LWDebugMode.dll"
#endif

typedef void lwAddLevelDebugFn(const char* pWorldName, const char* pLevelTitle, const char* pLevelId);

inline static void lwAddLevelDebug(const char* pWorldName, const char* pLevelTitle , const char* pLevelId)
{
	const HMODULE dbgModeHn = GetModuleHandleA(LW_DEBUG_MODE_DLL);
	if (!dbgModeHn)
		return;

	auto* pFunc = reinterpret_cast<lwAddLevelDebugFn*>(GetProcAddress(dbgModeHn, "AddLevelDebug"));
	if (!pFunc)
		return;

	pFunc(pWorldName, pLevelTitle, pLevelId);
}