#include "skse/PluginAPI.h"		// super
#include "skse/skse_version.h"	// What version of SKSE is running?
#include "skse\SafeWrite.h"
#include <shlobj.h>				// CSIDL_MYCODUMENTS
#include "skse/GameAPI.h"

void hookWaitTime();
void handleIniOptions();

float g_waitTime = 1;
HMODULE fasterSleepWaitHandle;

extern "C"	{

	BOOL WINAPI DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved) {
		if (dwReason == DLL_PROCESS_ATTACH)
			fasterSleepWaitHandle = (HMODULE)hDllHandle;
		return TRUE;
	}

	bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info) {

		// populate info structure
		info->infoVersion =	PluginInfo::kInfoVersion;
		info->name =		"Faster Sleep Wait";
		info->version =		1;

		if(skse->isEditor)
		{
			_MESSAGE("loaded in editor, marking as incompatible");

			return false;
		}
		else if(skse->runtimeVersion != RUNTIME_VERSION_1_9_32_0)
		{
			_MESSAGE("unsupported runtime version %08X", skse->runtimeVersion);

			return false;
		}
		return true;
	}

	bool SKSEPlugin_Load(const SKSEInterface * skse)	{	
		handleIniOptions();
		WriteRelJump(0x88810D, UInt32(hookWaitTime));
		return true;
	}

};

void __declspec(naked) hookWaitTime() {
	static const UInt32 retnAddr = 0x888112;
	__asm {
		fld g_waitTime
		mov[esi + 0x1C], eax
		jmp retnAddr
	}
}

void handleIniOptions() {
	char filename[MAX_PATH];
	GetModuleFileNameA(fasterSleepWaitHandle, filename, MAX_PATH);
	strcpy((char *)(strrchr(filename, '\\') + 1), "faster_sleep_wait.ini");
	g_waitTime = (float) GetPrivateProfileIntA("Main", "iWaitTimeInMS", 1000, filename)/1000.0;
}
