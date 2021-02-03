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

DWORD32 dwLocalPlayer;
DWORD32 dwEntityList;
DWORD32 mViewMatrix;

DWORD32 m_vecOrigin;
DWORD32 m_iHealth;
DWORD32 m_bDormant;
DWORD32 m_iTeamNum;
DWORD32 m_BoneMatrix;
DWORD32 m_aimPunchAngle;

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
	const char* file_name = ENCRYPT_STR_A(".\\config.ini");
	const char* section_name = ENCRYPT_STR_A("settings");

	if (!FileExists(file_name))
		return false;


	dwLocalPlayer = LI_FN(GetPrivateProfileIntA)(section_name, "dwLocalPlayer", 0, file_name);
	dwEntityList = LI_FN(GetPrivateProfileIntA)(section_name, "dwEntityList", 0, file_name);
	mViewMatrix = LI_FN(GetPrivateProfileIntA)(section_name, "mViewMatrix", 0, file_name);

	m_vecOrigin = LI_FN(GetPrivateProfileIntA)(section_name, "m_vecOrigin", 0, file_name);
	m_iHealth = LI_FN(GetPrivateProfileIntA)(section_name, "m_iHealth", 0, file_name);
	m_bDormant = LI_FN(GetPrivateProfileIntA)(section_name, "m_bDormant", 0, file_name);
	m_iTeamNum = LI_FN(GetPrivateProfileIntA)(section_name, "m_iTeamNum", 0, file_name);
	m_BoneMatrix = LI_FN(GetPrivateProfileIntA)(section_name, "m_BoneMatrix", 0, file_name);
	m_aimPunchAngle = LI_FN(GetPrivateProfileIntA)(section_name, "m_aimPunchAngle", 0, file_name);

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
	printf_s(ENCRYPT_STR_A("[GOD FACEIT] Created by @shockbyte\n"));
	printf_s(ENCRYPT_STR_A("[GOD FACEIT] Loading settings...\n"));
	if (!load_offsets())
	{
		printf_s(ENCRYPT_STR_A("[GOD FACEIT] Can't load settings. Check [settings.ini] file\n"));
		printf_s(ENCRYPT_STR_A("[GOD FACEIT] Closing...\n"));
		CloseConsole();
		return 0;
	}
	printf_s(ENCRYPT_STR_A("[GOD FACEIT] Loading driver...\n"));
	VM_END;
#pragma endregion 

#pragma region DRIVER_LOADING
	KernelInterface ring0;
	VM_START("WinMain_2");
	if (!ring0.NoErrors)
	{
		printf_s(ENCRYPT_STR_A("[GOD FACEIT] Can't load driver [0x%X]\n"), ring0.GetErrorCode());
		printf_s(ENCRYPT_STR_A("[GOD FACEIT] Closing...\n"));
		CloseConsole();
		return 0;
	}
	printf_s(ENCRYPT_STR_A("[GOD FACEIT] Driver loaded! Getting game info...\n"));
	while (!ring0.Attach()) {}
	while (!ring0.GetModules()) {}
	printf_s(ENCRYPT_STR_A("[GOD FACEIT] Setup is complete! Start rendering...\n"));
	CloseConsole();
	VM_END;
#pragma endregion
	StartRender(WINDOW_TITLE.c_str(), &ring0, hInstance);
	return 0;
}