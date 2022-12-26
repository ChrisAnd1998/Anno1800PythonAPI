#include "pch.h"
#include <string>
#include <tlhelp32.h>

HINSTANCE selfModuleHandle;
bool unload = false;

typedef int(__stdcall* PY_INITIALIZE)();
typedef int(__stdcall* PY_FINALIZE)();
typedef int(__stdcall* PY_ISINITIALIZED)();
typedef int(__stdcall* PYRUN_SIMPLESTRING)(const char* str);
typedef DWORD* (__stdcall* PYGILSTATE_ENSURE)();
typedef void(__stdcall* PYGILSTATE_RELEASE)(DWORD* gstate);

HMODULE hModule = GetModuleHandle(L"python35.dll");
PY_INITIALIZE Py_Initialize = (PY_INITIALIZE)GetProcAddress(hModule, "Py_Initialize");
PY_FINALIZE Py_Finalize = (PY_FINALIZE)GetProcAddress(hModule, "Py_Finalize");
PY_ISINITIALIZED Py_IsInitialized = (PY_ISINITIALIZED)GetProcAddress(hModule, "Py_IsInitialized");
PYGILSTATE_ENSURE PyGILState_Ensure = (PYGILSTATE_ENSURE)GetProcAddress(hModule, "PyGILState_Ensure");
PYGILSTATE_RELEASE PyGILState_Release = (PYGILSTATE_RELEASE)GetProcAddress(hModule, "PyGILState_Release");
PYRUN_SIMPLESTRING PyRun_SimpleString = (PYRUN_SIMPLESTRING)GetProcAddress(hModule, "PyRun_SimpleString");

BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		selfModuleHandle = hInst;

		Py_Initialize();

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
			std::size_t found = scriptPath.find_first_of("\\");
			while (found != std::string::npos)
			{
				scriptPath[found] = '/';
				found = scriptPath.find_first_of("\\", found + 1);
			}

			std::string command = "console.startScript(\"" + scriptPath + "\")";
			PyRun_SimpleString(command.c_str());
			PyGILState_Release(gstate);
		}

		Py_Finalize();

		bool b = true;
		while (b == true)
		{
			b = FreeLibrary(selfModuleHandle);
		}
	}

	return TRUE;
}