#include <Windows.h>
#include <SubAuth.h>
#include <comdef.h>

#include <ctime>
#include <iostream>

#include "SDK/globals.h"
#include "SDK/lazy_importer.hpp"
#include "SDK/VMProtectSDK.h"

#include "ring0/KernelInterface.h"
#include "render/render.h"

FILE* CON_OUT;

//Offsets
DWORD32 dwLocalPlayer;
DWORD32 dwEntityList;
DWORD32 dwViewMatrix;
DWORD32 dwClientState;


//Netvars
DWORD32 m_iHealth;
DWORD32 m_bDormant;
DWORD32 m_iTeamNum;
DWORD32 m_vecOrigin;
DWORD32 m_vecViewAngles;


//Settings
DWORD32 win_pos_x;
DWORD32 win_pos_y;

DWORD32 win_height;
DWORD32 win_width;

DWORD32 radar_zoom;
DWORD32 radar_point_size;

DWORD32 display_index;

void CloseConsole()
{
	LI_FN(Sleep)(3000);
	fclose(CON_OUT);
	LI_FN(FreeConsole)();
	PostMessageA(LI_FN(GetConsoleWindow)(), WM_CLOSE, 0, 0);
}

bool FileExists(const char* file)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE handle = LI_FN(FindFirstFileA)(file, &FindFileData);
	bool found = handle != INVALID_HANDLE_VALUE;
	if (found)
		LI_FN(FindClose)(handle);
	return found;
}

bool load_offsets()
{
	const char* file_name = ENCRYPT_STR_A(".\\bfg.ini");
	const char* offsets_section_name = ENCRYPT_STR_A("ofs");
	const char* netvars_section_name = ENCRYPT_STR_A("ntv");
	const char* config_section_name = ENCRYPT_STR_A("cfg");

	if (!FileExists(file_name))
		return false;


	//Load offsets
	dwLocalPlayer = LI_FN(GetPrivateProfileIntA)(offsets_section_name, "dwLocalPlayer", 0, file_name);
	dwEntityList = LI_FN(GetPrivateProfileIntA)(offsets_section_name, "dwEntityList", 0, file_name);
	dwViewMatrix = LI_FN(GetPrivateProfileIntA)(offsets_section_name, "dwViewMatrix", 0, file_name);
	dwClientState = LI_FN(GetPrivateProfileIntA)(offsets_section_name, "dwClientState", 0, file_name);
	

	//Load netvars
	m_iHealth = LI_FN(GetPrivateProfileIntA)(netvars_section_name, "m_iHealth", 0, file_name);
	m_bDormant = LI_FN(GetPrivateProfileIntA)(netvars_section_name, "m_bDormant", 0, file_name);
	m_iTeamNum = LI_FN(GetPrivateProfileIntA)(netvars_section_name, "m_iTeamNum", 0, file_name);
	m_vecOrigin = LI_FN(GetPrivateProfileIntA)(netvars_section_name, "m_vecOrigin", 0, file_name);
	m_vecViewAngles = LI_FN(GetPrivateProfileIntA)(netvars_section_name, "m_vecViewAngles", 0, file_name);


	//load settings
	win_pos_x = LI_FN(GetPrivateProfileIntA)(config_section_name, "win_pos_x", 0, file_name);
	win_pos_y = LI_FN(GetPrivateProfileIntA)(config_section_name, "win_pos_y", 0, file_name);
	
	win_height = LI_FN(GetPrivateProfileIntA)(config_section_name, "win_height", 0, file_name);
	win_width = LI_FN(GetPrivateProfileIntA)(config_section_name, "win_width", 0, file_name);
	
	display_index = LI_FN(GetPrivateProfileIntA)(config_section_name, "display_index", 0, file_name);
	
	radar_zoom = LI_FN(GetPrivateProfileIntA)(config_section_name, "radar_zoom", 0, file_name);
	radar_point_size = LI_FN(GetPrivateProfileIntA)(config_section_name, "radar_point_size", 0, file_name);
	
	return true;
}

std::string WINDOW_TITLE;

INT WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nShowCmd
)
{
	
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nShowCmd);
	
#pragma region SETTING_AND_WINDOW
	VM_START("WinMain_1");
	LI_FN(srand)(static_cast<unsigned>(GetTickCount64()));
	LI_FN(AllocConsole)();
	WINDOW_TITLE = "";
	for (size_t i = 0; i <= 16; i++)
		WINDOW_TITLE += static_cast<char>(rand() % ('Z' - 'A') + 'A');


	LI_FN(SetConsoleTitleA)(WINDOW_TITLE.c_str());
	freopen_s(&CON_OUT, ENCRYPT_STR_A("CONOUT$"), "w", stdout);
	printf_s(ENCRYPT_STR_A("[0074f0afb7c8e1fec29c10af1f0018a28e34207387640b9d5017d8e8d040d67a] Created by @shockbyte\n"));
	printf_s(ENCRYPT_STR_A("[0074f0afb7c8e1fec29c10af1f0018a28e34207387640b9d5017d8e8d040d67a] Loading settings...\n"));
	if (!load_offsets())
	{
		printf_s(ENCRYPT_STR_A("[0074f0afb7c8e1fec29c10af1f0018a28e34207387640b9d5017d8e8d040d67a] Can't load settings. Check [settings.ini] file\n"));
		printf_s(ENCRYPT_STR_A("[0074f0afb7c8e1fec29c10af1f0018a28e34207387640b9d5017d8e8d040d67a] Closing...\n"));
		CloseConsole();
		return 0;
	}
	printf_s(ENCRYPT_STR_A("[0074f0afb7c8e1fec29c10af1f0018a28e34207387640b9d5017d8e8d040d67a] Loading driver...\n"));
	VM_END;
#pragma endregion 

#pragma region DRIVER_LOADING
	KernelInterface ring0;
	VM_START("WinMain_2");
	if (!ring0.NoErrors)
	{
		printf_s(ENCRYPT_STR_A("[0074f0afb7c8e1fec29c10af1f0018a28e34207387640b9d5017d8e8d040d67a] Can't load driver [0x%X]\n"), ring0.GetErrorCode());
		printf_s(ENCRYPT_STR_A("[0074f0afb7c8e1fec29c10af1f0018a28e34207387640b9d5017d8e8d040d67a] Closing...\n"));
		CloseConsole();
		return 0;
	}
	printf_s(ENCRYPT_STR_A("[0074f0afb7c8e1fec29c10af1f0018a28e34207387640b9d5017d8e8d040d67a] Driver loaded! Getting game info...\n"));
	while (!ring0.Attach()) {}
	while (!ring0.GetModules()) {}
	printf_s(ENCRYPT_STR_A("[0074f0afb7c8e1fec29c10af1f0018a28e34207387640b9d5017d8e8d040d67a] Setup is complete!\n"));
	CloseConsole();
	VM_END;
#pragma endregion
	StartRender(WINDOW_TITLE.c_str(), &ring0, hInstance);
	return 0;
}