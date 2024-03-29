#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include "include/detours.h"
#include "include/detver.h"
#include "include/syelog.h"
#include "include/INIReader.h"

#define HOOK(returnType, callingConvention, functionName, location, ...) \
    typedef returnType callingConvention functionName(__VA_ARGS__); \
    functionName* original##functionName = (functionName*)(location); \
    returnType callingConvention implOf##functionName(__VA_ARGS__)

#define INSTALL_HOOK(functionName) \
	{ \
		DetourTransactionBegin(); \
		DetourUpdateThread(GetCurrentThread()); \
		DetourAttach((void**)&original##functionName, implOf##functionName); \
		DetourTransactionCommit(); \
	}

#define VTABLE_HOOK(returnType, callingConvention, className, functionName, ...) \
	typedef returnType callingConvention functionName(className* This, __VA_ARGS__); \
	functionName* original##functionName; \
	returnType callingConvention implOf##functionName(className* This, __VA_ARGS__)

#define INSTALL_VTABLE_HOOK(object, functionName, functionIndex) \
	{ \
		void** addr = &(*(void***)object)[functionIndex]; \
		if (*addr != implOf##functionName) \
		{ \
			original##functionName = (functionName*)*addr; \
			DWORD oldProtect; \
			VirtualProtect(addr, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect); \
			*addr = implOf##functionName; \
			VirtualProtect(addr, sizeof(void*), oldProtect, NULL); \
		} \
	}

#define WRITE_CALL(location, FUNC) \
    { \
        WRITE_MEMORY_TYPE(location, uint8_t, 0xE8); \
        WRITE_MEMORY_TYPE((location + 1), size_t, (size_t)((void*)(FUNC)) - (size_t)(location) - 5); \
    }

#define WRITE_MEMORY_TYPE(location, TYPE, ...) \
	{ \
		const TYPE data[] = { __VA_ARGS__ }; \
		DWORD oldProtect; \
		VirtualProtect((void*)location, sizeof(data), PAGE_EXECUTE_READWRITE, &oldProtect); \
		memcpy((void*)location, data, sizeof(data)); \
		VirtualProtect((void*)location, sizeof(data), oldProtect, NULL); \
	}

#define WRITE_MEMORY(location, ...) WRITE_MEMORY_TYPE(location, uint8_t, __VA_ARGS__)

#define WRITE_FUNCTION(location, func)\
	{ \
		DWORD oldProtect; \
		void* address = (void*)func; \
		VirtualProtect((void*)location, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect); \
		memcpy((void*)location, &address, sizeof(void*)); \
		VirtualProtect((void*)location, sizeof(void*), oldProtect, NULL); \
	}