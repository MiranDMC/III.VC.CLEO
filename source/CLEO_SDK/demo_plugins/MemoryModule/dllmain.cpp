#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "MemoryModule.h"

#if CLEO_VC
#pragma comment (lib, "VC.CLEO.lib")
#include "VC.CLEO.h"
#else
#pragma comment (lib, "III.CLEO.lib")
#include "III.CLEO.h"
#endif

#define CLEO_VERSION_MAIN    2
#define CLEO_VERSION_MAJOR   0
#define CLEO_VERSION_MINOR   1

#define CLEO_VERSION ((CLEO_VERSION_MAIN << 24)|(CLEO_VERSION_MAJOR << 16)|(CLEO_VERSION_MINOR << 8))

tScriptVar *Params;

//0BA2=2,%2h% = memory_load_library %1s% ; IF and SET
eOpcodeResult WINAPI OPCODE_0BA2(CScript *script)
{
	script->Collect(1);
	auto libHandle = MemoryLoadLibrary(Params[0].cVar);
	Params[0].pVar = libHandle;
	script->Store(1);
	script->UpdateCompareFlag(libHandle != nullptr);
	return OR_CONTINUE;
}

//0BA3=1, memory_free_library %1h%
eOpcodeResult WINAPI OPCODE_0BA3(CScript *script)
{
	script->Collect(1);
	HMEMORYMODULE libHandle;
	libHandle = (HMEMORYMODULE)Params[0].pVar;
	MemoryFreeLibrary(libHandle);
	return OR_CONTINUE;
}

//0BA4=3,%3d% = memory_get_proc_address %1s% library %2d% ; IF and SET
eOpcodeResult WINAPI OPCODE_0BA4(CScript *script)
{
	script->Collect(2);
	char *funcName = Params[0].cVar;
	HMEMORYMODULE libHandle;
	libHandle = (HMEMORYMODULE)Params[1].pVar;
	void *funcAddr = (void *)MemoryGetProcAddress(libHandle, funcName);
	Params[0].pVar = funcAddr;
	script->Store(1);
	script->UpdateCompareFlag(funcAddr != nullptr);
	return OR_CONTINUE;
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		if (CLEO_GetVersion() < CLEO_VERSION)
		{
			MessageBox(HWND_DESKTOP, "An incorrect version of CLEO was loaded.", "MemoryModule.cleo", MB_ICONERROR);
			return FALSE;
		}

		Params = CLEO_GetParamsAddress();
		Opcodes::RegisterOpcode(0x0BA2, OPCODE_0BA2);
		Opcodes::RegisterOpcode(0x0BA3, OPCODE_0BA3);
		Opcodes::RegisterOpcode(0x0BA4, OPCODE_0BA4);
	}
	return TRUE;
}