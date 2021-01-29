#include <ntddk.h>

NTSTATUS DriverEP(
	PDRIVER_OBJECT  DriverObject,
	PUNICODE_STRING RegistryPath
);

#pragma alloc_text(INIT, DriverEP)

NTSTATUS DriverEP(
	PDRIVER_OBJECT  DriverObject,
	PUNICODE_STRING RegistryPath
)
{
	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(RegistryPath);
	return STATUS_SUCCESS;
}