#include "pch.h"
#include <string>
#include <tlhelp32.h>
#include <thread>

HINSTANCE selfModuleHandle;
bool unload = false;

typedef DWORD* (__stdcall* PYEVAL_SAVETHREAD)();
typedef int(__stdcall* PYEVAL_INITTHREADS)();
typedef int(__stdcall* PY_INITIALIZEEX)(int threadstate);
typedef int(__stdcall* PY_INITIALIZE)();
typedef int(__stdcall* PY_FINALIZE)();
typedef int(__stdcall* PY_ISINITIALIZED)();
typedef int(__stdcall* PYRUN_SIMPLESTRING)(const char* str);
typedef int(__stdcall* PYRUN_SIMPLEFILE)(FILE* fp, const char* filename);
typedef DWORD* (__stdcall* PYGILSTATE_ENSURE)();
typedef void(__stdcall* PYGILSTATE_RELEASE)(DWORD* gstate);
typedef void(__stdcall* PYTHREADSTATE_SWAP)(DWORD* threadstate);

HMODULE hModule = GetModuleHandle(L"python35.dll");
PYEVAL_SAVETHREAD PyEval_SaveThread = (PYEVAL_SAVETHREAD)GetProcAddress(hModule, "PyEval_SaveThread");
PYEVAL_INITTHREADS PyEval_InitThreads = (PYEVAL_INITTHREADS)GetProcAddress(hModule, "PyEval_InitThreads");
PY_INITIALIZEEX Py_InitializeEx = (PY_INITIALIZEEX)GetProcAddress(hModule, "Py_InitializeEx");
PY_INITIALIZE Py_Initialize = (PY_INITIALIZE)GetProcAddress(hModule, "Py_Initialize");
PY_FINALIZE Py_Finalize = (PY_FINALIZE)GetProcAddress(hModule, "Py_Finalize");
PY_ISINITIALIZED Py_IsInitialized = (PY_ISINITIALIZED)GetProcAddress(hModule, "Py_IsInitialized");
PYGILSTATE_ENSURE PyGILState_Ensure = (PYGILSTATE_ENSURE)GetProcAddress(hModule, "PyGILState_Ensure");
PYGILSTATE_RELEASE PyGILState_Release = (PYGILSTATE_RELEASE)GetProcAddress(hModule, "PyGILState_Release");
PYTHREADSTATE_SWAP PyThreadState_Swap = (PYTHREADSTATE_SWAP)GetProcAddress(hModule, "PyThreadState_Swap");
PYRUN_SIMPLEFILE PyRun_SimpleFile = (PYRUN_SIMPLEFILE)GetProcAddress(hModule, "PyRun_SimpleFile");
PYRUN_SIMPLESTRING PyRun_SimpleString = (PYRUN_SIMPLESTRING)GetProcAddress(hModule, "PyRun_SimpleString");

int get_file_size(std::string filename)
{
	FILE* p_file = NULL;
	fopen_s(&p_file, filename.c_str(), "rb");
	fseek(p_file, 0, SEEK_END);
	int size = ftell(p_file);
	fclose(p_file);
	return size;
}


BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		selfModuleHandle = hInst;

		
		Py_Initialize();

		//PyEval_InitThreads();
		//DWORD* thread_state = PyEval_SaveThread();

		if (Py_IsInitialized != 0)
		{
			DWORD* gstate = PyGILState_Ensure();

			char buffer[MAX_PATH];
			HMODULE AnnoPythonInjectModule = GetModuleHandle(L"annopythoninject.dll");
			GetModuleFileNameA(AnnoPythonInjectModule, buffer, sizeof(buffer));
			std::string::size_type pos = std::string(buffer).find_last_of("\\/") - 1;
			std::string path = std::string(buffer);

			path.replace(path.find("AnnoPythonInject.dll"), sizeof("AnnoPythonInject.dll") - 1, "script.lua");

			if (get_file_size(path) == 2)
			{
				path.replace(path.find("script.lua"), sizeof("script.lua") - 1, "script.py");
			}

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
			//PyThreadState_Swap(thread_state);
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