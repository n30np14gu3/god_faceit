#include "globals.h"

NTSTATUS KeReadVirtualMemory32(PEPROCESS Process, DWORD32 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T ReadedBytes);
NTSTATUS KeWriteVirtualMemory32(PEPROCESS Process, DWORD32 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T WritedBytes);


void InitCheatData(PIRP Irp);
void GetClientDll(PIRP Irp);

NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	
	ULONG bytesIO = 0;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	const ULONG controlCode = stack->Parameters.DeviceIoControl.IoControlCode;
	
	PKERNEL_WRITE_REQUEST32 pWriteRequest32;
	PKERNEL_READ_REQUEST32 pReadRequest32;

	SIZE_T rwBytes = 0;
	switch (controlCode)
	{
	case IO_INIT_CHEAT_DATA:
		DPRINT("IO_INIT_CHEAT_DATA");
		InitCheatData(Irp);
		bytesIO = sizeof(KERNEL_INIT_DATA_REQUEST);
		break;
		
	case IO_READ_PROCESS_MEMORY_32:
		if (!DRIVER_INITED)
			break;

		pReadRequest32 = (PKERNEL_READ_REQUEST32)Irp->AssociatedIrp.SystemBuffer;
		if (pReadRequest32 != NULL)
		{
			pReadRequest32->Result = KeReadVirtualMemory32(PEGAME_PROCESS, pReadRequest32->Address, (pReadRequest32->Response), pReadRequest32->Size, &rwBytes);
		}
		bytesIO = sizeof(KERNEL_READ_REQUEST32);
		break;

	case IO_WRITE_PROCESS_MEMORY_32:
		if (!DRIVER_INITED)
			break;

		pWriteRequest32 = (PKERNEL_WRITE_REQUEST32)Irp->AssociatedIrp.SystemBuffer;
		if (pWriteRequest32 != NULL)
		{
			pWriteRequest32->Result = KeWriteVirtualMemory32(PEGAME_PROCESS, pWriteRequest32->Address, pWriteRequest32->Value, pWriteRequest32->Size, &rwBytes);
		}
		bytesIO = sizeof(KERNEL_WRITE_REQUEST32);
		break;

	case IO_GET_CLIENT_DLL:
		if (!DRIVER_INITED)
			break;
		GetClientDll(Irp);
		bytesIO = sizeof(KERNEL_GET_CLIENT_DLL);
		break;
	default:break;
	}
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = bytesIO;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

void GetClientDll(PIRP Irp)
{
	VM_START("#GetClientDll");
	PKERNEL_GET_CLIENT_DLL pModules = (PKERNEL_GET_CLIENT_DLL)Irp->AssociatedIrp.SystemBuffer;
	if (pModules != NULL)
	{
		if (CLIENT_DLL_BASE == 0)
			pModules->result = STATUS_ACCESS_DENIED;
		else
		{
			pModules->bClient = CLIENT_DLL_BASE;
			pModules->result = STATUS_SUCCESS;
		}
	}
	VM_END;
}

void InitCheatData(PIRP Irp)
{
	VM_START("#InitCheatData");
	PKERNEL_INIT_DATA_REQUEST pInitData;
	pInitData = (PKERNEL_INIT_DATA_REQUEST)Irp->AssociatedIrp.SystemBuffer;
	DPRINT("pInitData->CheatId = 0x%X", (DWORD32)pInitData->CheatId);
	DPRINT("pInitData->CheatId = 0x%X", (DWORD32)pInitData->CsgoId);
	if (!DRIVER_INITED)
	{
		GAME_PROCESS = (HANDLE)pInitData->CsgoId;
		PROTECTED_PROCESS = (HANDLE)pInitData->CheatId;
		pInitData->Result = PsLookupProcessByProcessId(GAME_PROCESS, &PEGAME_PROCESS);
		pInitData->Result |= PsLookupProcessByProcessId(PROTECTED_PROCESS, &PEPROTECTED_PROCESS);
		DRIVER_INITED = NT_SUCCESS(pInitData->Result);
		DPRINT("pInitDate->Result = 0x$X", pInitData->Result);
	}
	else
		pInitData->Result = STATUS_SUCCESS;

	VM_END;
}

NTSTATUS KeReadVirtualMemory32(PEPROCESS Process, DWORD32 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T ReadedBytes)
{
	if (!MemCopy)
		return STATUS_NOT_FOUND;
	
	return MemCopy(
		Process,
		(PVOID)SourceAddress,
		PEPROTECTED_PROCESS,
		(PVOID64)TargetAddress,
		Size,
		KernelMode,
		ReadedBytes);
}

NTSTATUS KeWriteVirtualMemory32(PEPROCESS Process, DWORD32 SourceAddress, DWORD64 TargetAddress, SIZE_T Size, PSIZE_T WritedBytes)
{
	if (!MemCopy)
		return STATUS_NOT_FOUND;
	
	return MemCopy(
		PEPROTECTED_PROCESS,
		(PVOID64)TargetAddress,
		Process,
		(PVOID)SourceAddress,
		Size,
		KernelMode,
		WritedBytes);
}
