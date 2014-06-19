#include "Required.h"

std::string g_Directory;
Logger<char>::Stream logstream;

BOOL WINAPI DllMain(HINSTANCE hInstance, UINT_PTR dwReasonOfCall, LPVOID lpReserved)
{
	if(dwReasonOfCall == DLL_PROCESS_ATTACH)
	{
		g_Directory = DirModule(hInstance);
		logstream.open(Logger<char>());    // Opens the logging stream
		std::cout.rdbuf(logstream.rdbuf());   // Initializes the logging stream
		CreateThread(0, 0, HookThread, 0, 0, 0);   // Launches the main hook thread
	}

	return TRUE; // Success
}