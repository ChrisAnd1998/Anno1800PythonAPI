#include "pch.h"
#include <string>
#include <tlhelp32.h>
#include <regex>

BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
        _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);

        HMODULE hModule;
        HINSTANCE selfModuleHandle;
        bool unload = false;

        selfModuleHandle = hInst;

        typedef int(__stdcall* PY_ISINITIALIZED)();
        typedef int(__stdcall* PYRUN_SIMPLESTRING)(const char* str);
        typedef DWORD* (__stdcall* PYGILSTATE_ENSURE)();
        typedef void(__stdcall* PYGILSTATE_RELEASE)(DWORD* gstate);

        PY_ISINITIALIZED Py_IsInitialized;
        PYRUN_SIMPLESTRING PyRun_SimpleString;
        PYGILSTATE_ENSURE PyGILState_Ensure;
        PYGILSTATE_RELEASE PyGILState_Release;

        hModule = GetModuleHandle(L"python35.dll");
        Py_IsInitialized = (PY_ISINITIALIZED)GetProcAddress(hModule, "Py_IsInitialized");
        PyGILState_Ensure = (PYGILSTATE_ENSURE)GetProcAddress(hModule, "PyGILState_Ensure");
        PyGILState_Release = (PYGILSTATE_RELEASE)GetProcAddress(hModule, "PyGILState_Release");
        PyRun_SimpleString = (PYRUN_SIMPLESTRING)GetProcAddress(hModule, "PyRun_SimpleString");

        if (Py_IsInitialized != 0)
        {
            DWORD* gstate = PyGILState_Ensure();

            char buffer[MAX_PATH];
            HMODULE AnnoPythonInjectModule = GetModuleHandle(L"annopythoninject.dll");
            GetModuleFileNameA(AnnoPythonInjectModule, buffer, sizeof(buffer));
            std::string::size_type pos = std::string(buffer).find_last_of("\\/") - 1;
            std::string path = std::string(buffer);
            path.replace(path.find("AnnoPythonInject.dll"), sizeof("AnnoPythonInject.dll") - 1, "script.lua");
          
            std::string scriptPath = path;
            std::string scriptPath2 = std::regex_replace(scriptPath, std::regex("\\\\"), "/");
            std::string command = "console.startScript(\"" + scriptPath2 + "\")";
            PyRun_SimpleString(command.c_str());
            PyGILState_Release(gstate);
        }

        free(Py_IsInitialized);
        free(PyGILState_Ensure);
        free(PyGILState_Release);
        free(PyRun_SimpleString);

        bool b = true;
        while (b == true)
        {
            b = FreeLibrary(selfModuleHandle);
        }


    }



    return TRUE;
}

