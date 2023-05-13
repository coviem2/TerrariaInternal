// dllmain.cpp : Defines the entry point for the DLL application.
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#include <thread>
#include <cstdint>
#include <iostream>
#include <vector>
#include "hooks.h"
#include "../ext/minhook/minhook.h"

HMODULE instance;

DWORD WINAPI HackThread()
{
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    std::cout << "Gottem." << std::endl;

    bool worked = true;
    try
    {
        gui::Setup();
        hooks::Setup();

    }
    catch (const std::exception& error)
    {
        MessageBox(0, error.what(), "Error", MB_OK | MB_ICONEXCLAMATION);
        worked = false;
    }

    while (worked)
    {
        Sleep(500);
        if (GetAsyncKeyState(VK_DELETE))
        {
            break;
        }
        if (GetAsyncKeyState(VK_NUMPAD1))
        {
            std::cout << std::hex << hooks::get_LocalPlayer() << std::endl;
        }
    }

    hooks::Destroy();
    gui::Destroy();

    if (f != NULL)
    {
        fclose(f);
    }
    FreeConsole();
    FreeLibraryAndExitThread(instance, 0);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        instance = hModule;
        const HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, NULL);

        if (thread)
        {
            CloseHandle(thread);
        }
    }
    return TRUE;
}

