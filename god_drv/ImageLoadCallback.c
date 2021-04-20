#include "globals.h"
#include "global_defs.h"
#include "ImageLoadCallback.h"

void ImageLoadCallback(PUNICODE_STRING FullImageName, HANDLE ProcessId, PIMAGE_INFO ImageInfo)
{
	VM_START("#ImageLoadCallback");
	UNREFERENCED_PARAMETER(ProcessId);
	if (wcsstr(FullImageName->Buffer, CLIENT_DLL))
	{
		CLIENT_DLL_BASE = (DWORD32)(DWORD64)ImageInfo->ImageBase;
		DPRINT("Loading client.dll 0x%X", CLIENT_DLL_BASE);
		return;
	}

	if (wcsstr(FullImageName->Buffer, ENGINE_DLL))
	{
		ENGINE_DLL_BASE = (DWORD32)(DWORD64)ImageInfo->ImageBase;
		DPRINT("Loading engine.dll 0x%X", ENGINE_DLL_BASE);
	}
	VM_END;
}
