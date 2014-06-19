#include "Required.h"
//--Typedefs
typedef ClientClass* (__thiscall* GetAllClasses_t)(PVOID pThis);
typedef void* (*CI)(const char* pName, int* pReturnCode);
//--Variables
PVOID ClientInterface = NULL;
int cliversion;

void GetClientInterface(HMODULE Client)
{
	CI ClientFactory = reinterpret_cast<CI>(GetProcAddress(Client, "CreateInterface"));

	for(short version = 0; version < 99; version ++)
	{
		std::ostringstream o;
		o  << "VClient" << std::setfill('0') << std::setw(3) << version;
		ClientInterface = ClientFactory(o.str().c_str(), NULL);

		if(ClientInterface)
		{
			std::cout << "Client version string: " << o.str() << std::endl;
			cliversion = version;
			break;
		}
	}
}

DWORD WINAPI HookThread(LPVOID lpArguments)
{
	GetAllClasses_t gac;
	int index = 0;
	GetClientInterface(GetModuleHandleSafe("client"));

	//--
	switch(cliversion)
	{
	case 11:
	case 12:
	case 13: // CSS Engine
		index = 5;
		std::cout << "Source detected" << std::endl;
		break;

	case 15: // OB Engine
		index = 6;
		std::cout << "OB detected" << std::endl;
		break;

	case 16: // L4D 1
		index = 7;
		std::cout << "L4D1 detected" << std::endl;
		break;

	case 17: // L4D 2
		index = 3;
		std::cout << "L4D2 detected" << std::endl;
		break;
	}

	//--
	gac = reinterpret_cast<GetAllClasses_t>(GetFuncAdd(ClientInterface, index));
	//--
	std::cout << "Dumping to " << g_Directory << "netvars.h" << std::endl;
	//--
	NetVars::Analyze(gac(ClientInterface));
	NetVars::GenCode((g_Directory + "netvars.h").c_str());
	//--
	std::cout << "NetVars dumped" << std::endl;
	return TRUE;
}