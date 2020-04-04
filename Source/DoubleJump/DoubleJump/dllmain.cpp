// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "LostCodeLoader.h"
#include "Types.h"
#include "INIReader.h"

bool canJump = false;
bool colorsJump = false;
bool AButtonHoming = false;
float jumpPower = 15.0f;
float underwaterPower = 10.0f;

HOOK(char, __stdcall, SonicStateGrounded, 0xDFF660, int* a1, bool a2)
{
	canJump = true;
	return originalSonicStateGrounded(a1, a2);
}

void* changeStateOriginal = (void*)0xE4FF30;

void __cdecl ChangeState(GensString* state, int* context)
{
	__asm
	{
		mov eax, state
		mov ecx, context
		call[changeStateOriginal]
	}
}

char __cdecl DoLightDash(int* a1, int* context)
{
	bool* stateFlags = (bool*)*(int*)(context[0x14D] + 4);
	char grounded = (char)context[0xD8];
	bool underwater = stateFlags[0x24];

	if (context[0x4AF])
	{
		context[0x4B0] = 0;
		ChangeState(&GensString("LightSpeedDash"), context);
		return true;
	}
	else if (!grounded && !colorsJump)
	{
		if (underwater)
		{
			canJump = false;
			((float*)context)[0xA5] = underwaterPower;
			ChangeState(&GensString("Jump"), context);
		}
		else if (canJump)
		{
			canJump = false;
			((float*)context)[0xA5] = jumpPower;
			ChangeState(&GensString("Jump"), context);
		}
		return true;
	}
	return false;
}

char __cdecl JumpStateHook(int* context)
{
	bool* stateFlags = (bool*)*(int*)(context[0x14D] + 4);
	bool underwater = stateFlags[0x24];
	if (colorsJump)
	{
		if (context[0x3A6] && AButtonHoming)
		{
			ChangeState(&GensString("HomingAttack"), context);
			return true;
		}
		if (underwater)
		{
			canJump = false;
			((float*)context)[0xA5] = underwaterPower;
			ChangeState(&GensString("Jump"), context);
		}
		else if (canJump)
		{
			canJump = false;
			((float*)context)[0xA5] = jumpPower;
			ChangeState(&GensString("Jump"), context);
		}
	} 
	else if (AButtonHoming)
	{
		ChangeState(&GensString("HomingAttack"), context);
	}
	return true;
}

__declspec(naked) void LightDashHook()
{
	__asm
	{
		push esi
		push ecx
		call DoLightDash
		add esp, 8
		ret
	}
}

int IsButtonPressed = 0xD97E00;

__declspec(naked) void JumpStateMidAsmHook()
{
	__asm
	{
		mov AButtonHoming, al
		mov eax, [esi + 11Ch]
		push edi
		xor edi, edi
		call [IsButtonPressed]
		pop edi
		test al, al
		mov eax, 0
		jz return
		
		push esi
		call JumpStateHook
		add esp, 4

		return:
		pop esi
		pop ecx
		retn 4
	}
}

extern "C"
{
	__declspec(dllexport) void Init(ModInfo* info)
	{
		INSTALL_HOOK(SonicStateGrounded);
		WRITE_JUMP(0x00DFB3F0, &LightDashHook);
		WRITE_JUMP(0x00DFFEA3, &JumpStateMidAsmHook);
		INIReader reader("config.ini");
		jumpPower = reader.GetFloat("Main", "JumpPower", jumpPower);
		underwaterPower = reader.GetFloat("Main", "UnderwaterJumpPower", underwaterPower);
		colorsJump = reader.GetBoolean("Main", "ColorsJump", colorsJump);
	}
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

