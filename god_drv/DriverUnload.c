#include "globals.h"

#include "ImageLoadCallback.h"
#include "CreateProcessCallback.h"
#include "callback.h"

void UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);
	DisableCallback();
	IoDeleteSymbolicLink(&DosName);
	IoDeleteDevice(pDriverObject->DeviceObject);
	PsRemoveLoadImageNotifyRoutine(ImageLoadCallback);
	PsSetCreateProcessNotifyRoutine(CreateProcessCallback, TRUE);
	DPRINT("[GOD FACEIT] Driver Unloaded!");
}
