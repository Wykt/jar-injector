#include "injector.hpp"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{

    if (ul_reason_for_call == DLL_PROCESS_ATTACH) 
    {
        DisableThreadLibraryCalls(hModule);

        HANDLE thread_handle = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(injector::main_thread), hModule, 0, nullptr);
        
        if (thread_handle)
        {
            CloseHandle(thread_handle);
        }
    }

    return TRUE;
}

