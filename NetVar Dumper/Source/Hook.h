#ifndef Hook_h__
#define Hook_h__

#include "Required.h"
// Main thread
DWORD WINAPI HookThread(LPVOID lpArguments);
//--Utility functions
inline HMODULE GetModuleHandleSafe(const char* ModuleName)
{
	HMODULE Ret = NULL;

	while(Ret == NULL)
	{
		Ret = GetModuleHandleA(ModuleName);

		if(Ret == NULL)
		{
			Sleep(100);
		}
	}

	return Ret;
}

inline UINT_PTR GetFuncAdd(PVOID pClass, int index)
{
	try
	{
		PUINT_PTR ClassTable = *reinterpret_cast<PUINT_PTR*>(pClass);
		return ClassTable[ index ];
	}
	catch(...)
	{
		return 0;
	}
}

inline std::string DirModule(HMODULE module)
{
	using std::string;
	char* temp = reinterpret_cast<char*>(calloc(MAX_PATH, sizeof(char)));
	GetModuleFileNameA(module, temp, MAX_PATH);
	string path(temp);
	delete [] temp;
	string::size_type lastsl = path.find_last_of('\\');

	if(lastsl == string::npos)
	{
		return "";
	}

	return path.substr(0, lastsl + 1);
}

#endif // Hook_h__