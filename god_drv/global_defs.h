#pragma once
#define PROCESS_QUERY_LIMITED_INFORMATION      0x1000

#define DRIVER_NAME			L"nova_ac"

#define DEVICE_NAME			ENCRYPT_STR_W(L"\\Device\\" DRIVER_NAME)
#define SYMBOL_NAME			ENCRYPT_STR_W(L"\\DosDevices\\" DRIVER_NAME)

#define CLIENT_DLL			ENCRYPT_STR_W(L"\\Counter-Strike Global Offensive\\csgo\\bin\\client.dll")
#define ENGINE_DLL			ENCRYPT_STR_W(L"\\Counter-Strike Global Offensive\\bin\\engine.dll")

#define COPY_MEMORY_ROUTINE	ENCRYPT_STR_W(L"MmCopyVirtualMemory")
