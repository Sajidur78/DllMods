#pragma once
#include "DbgWindowManager.h"

namespace app::dbg
{
	static FUNCTION_PTR(void, __thiscall, dbgFormObjectCtor, ASLR(0x004450E0), void* mem, const char* pName);
	static FUNCTION_PTR(void, __thiscall, gindowsAddControl, ASLR(0x00971380), void* form, void* control);
	static FUNCTION_PTR(void, __thiscall, gindowsControlSetName, ASLR(0x00971890), void* mem, const char* name);
	static FUNCTION_PTR(void, __thiscall, CReflectionEditControlCtor, ASLR(0x004591B0), void* mem, void* data);

	inline static FUNCTION_PTR(void, __thiscall, gindowsControlSetSize, ASLR(0x0096FFD0), void* pControl, const csl::ut::Size2<int>& size);
	inline static FUNCTION_PTR(void, __thiscall, gindowsControlSetLocaion, ASLR(0x00971630), void* pControl, const csl::ut::Point2<int>& point);
	
	static gindows::Control* CreateDevConfigWindow()
	{
		void* devConfig = gindows::GetMemoryAllocator()->Alloc(2048, 32);
		dbgFormObjectCtor(devConfig, "DevConfig");

		const csl::ut::Size2<int> size{ 480, 240 };
		gindowsControlSetSize(devConfig, size);

		const csl::ut::Point2<int> point{ 700, 100 };
		gindowsControlSetLocaion(devConfig, point);

		void* reflectionEdit = gindows::GetMemoryAllocator()->Alloc(588, 16);
		CReflectionEditControlCtor(reflectionEdit, nullptr);

		static_cast<size_t*>(devConfig)[155] = reinterpret_cast<size_t>(reflectionEdit);

		gindowsAddControl(devConfig, reflectionEdit);
		return static_cast<gindows::Control*>(devConfig);
	}
	
	inline static WindowInitNode ms_devConfigWindow{ "DevConfig", CreateDevConfigWindow };
}
