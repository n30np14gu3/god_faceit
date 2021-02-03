#include "globals.h"
#include "global_defs.h"
#include "functions.h"

#include "callback.h"
#include "ImageLoadCallback.h"
#include "CreateProcessCallback.h"

PDEVICE_OBJECT pDeviceObj;

UNICODE_STRING DeviceName;
UNICODE_STRING DosName;


//Cheat Process
HANDLE PROTECTED_PROCESS;
PEPROCESS PEPROTECTED_PROCESS;

//CSGO Process
HANDLE GAME_PROCESS;
PEPROCESS PEGAME_PROCESS;

//Client.dll
DWORD32 CLIENT_DLL_BASE;

BOOLEAN DRIVER_INITED;

PDRIVER_OBJECT g_Driver;

MmCopyVirtualMemoryFn MemCopy;
UNICODE_STRING MemCopyRoutineName;

NTSTATUS DriverEP(
	PDRIVER_OBJECT  DriverObject,
	PUNICODE_STRING RegistryPath
)
{
	VM_START("DriverEP");
	UNREFERENCED_PARAMETER(RegistryPath);

	DPRINT("[GOD FACEIT] Loading...");
	
	DriverObject->DriverUnload = UnloadDriver;


	for (ULONG t = 0; t < IRP_MJ_MAXIMUM_FUNCTION; t++)
		DriverObject->MajorFunction[t] = UnsupportedDispatch;

	DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCall;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseCall;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;

	DPRINT("[GOD FACEIT] Createing device...");

	RtlSecureZeroMemory(&DeviceName, sizeof(DeviceName));
	RtlInitUnicodeString(&DeviceName, DEVICE_NAME);
	RtlSecureZeroMemory(&DosName, sizeof(DosName));
	RtlInitUnicodeString(&DosName, SYMBOL_NAME);

	NTSTATUS status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE,
	                                 &pDeviceObj);
	IoCreateSymbolicLink(&DosName, &DeviceName);

	if (!NT_SUCCESS(status))
	{
		DPRINT("[DOG FACEIT] Creating device Error: Code 0x%X", status);
		return status;
	}

	if (pDeviceObj != NULL)
	{
		pDeviceObj->Flags |= DO_DIRECT_IO;
		pDeviceObj->Flags &= ~DO_DEVICE_INITIALIZING;
	}

	DPRINT("[GOD FACEIT] Getting routines...");
	RtlSecureZeroMemory(&MemCopyRoutineName, sizeof(MemCopyRoutineName));
	RtlInitUnicodeString(&MemCopyRoutineName, COPY_MEMORY_ROUTINE);
	MemCopy = (MmCopyVirtualMemoryFn)MmGetSystemRoutineAddress(&MemCopyRoutineName);
	if(MemCopy == NULL)
	{
		DPRINT("[GOD FACEIT] Can't find %s Routine!", COPY_MEMORY_ROUTINE);
		return STATUS_NOT_FOUND;
	}

	DPRINT("[GOD FACEIT] Setup callbacks...");
	PsSetLoadImageNotifyRoutine(ImageLoadCallback);
	PsSetCreateProcessNotifyRoutine(CreateProcessCallback, FALSE);
	
	VM_END;
	EnableCallback();
	DPRINT("[GOD FACEIT] Driver Loaded!");
	return STATUS_SUCCESS;
}