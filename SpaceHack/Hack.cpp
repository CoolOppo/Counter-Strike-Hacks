/*
# SpaceHack CS:GO base
# Coded by Sandaasu & Grab

credits :
- Grab
- UC
*/

#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <TlHelp32.h>
using std::cout;
using std::cin;
#include <limits>
using std::numeric_limits;
using std::streamsize;
using namespace std;
#include <cstdlib>

DWORD GetModuleBase (const DWORD dwProcessId, const char* szModuleName);
using namespace std;


int hack()
{

	DWORD pID;
	HANDLE hHandle;
	HWND hWnd;
	SetConsoleTitle ("SpaceHack BASE by sandaasu");
	hWnd = FindWindow (0, "Counter-Strike: Global Offensive");
	GetWindowThreadProcessId (hWnd, &pID);
	hHandle = OpenProcess (PROCESS_ALL_ACCESS, 0, pID);

	if (hHandle)
		{
	  	if (!hWnd)
				{

					return 0;


				}
			else
				{


					system ("cls");
					system ("Color 8A");
					cout << "..... Game found ! :) " << "\n "/*<< "\nhWnd: " << hWnd */ << endl;
					cout << "Loading features..." << "\n" << endl;
					//Laden der Engine.dll
					DWORD EngineDll;
					EngineDll = GetModuleBase (pID, "engine.dll");
					/////////////////////////////////////////////////
					//Laden der Client.dll
					DWORD ClientDll;
					ClientDll = GetModuleBase (pID, "client.dll");
					//cout << "Client.dll: " << ClientDll << "\n \n" << endl;
					/////////////////////////////////////////////////
					//Laden der materialsystem.dll
					DWORD materialsystemDll;
					materialsystemDll = GetModuleBase (pID, "materialsystem.dll");
					/////////////////////////////////////////////////



					/*			FUNKTIONEN							*/

					// SV_CHEATS Bypass
					int bypass_value = 1;
					DWORD ibypass_value = sizeof (bypass_value);
					WriteProcessMemory (hHandle, (LPVOID) (EngineDll + 0x674B98), &bypass_value, ibypass_value, 0);//EngineDll+674B98
					cout << "SV_Cheats bypass =  OK! " << "\n" << endl;



					// Gates
					/*int gates_value = 1;
					DWORD igates_value = sizeof(gates_value);
					WriteProcessMemory( hHandle, (LPVOID)( EngineDll + 0x624410), &gates_value, igates_value, 0);//engine.dll+624410
					cout << "gates = OK " << "\n" << endl;
					*/

					// r_drawothermodels 2
					int wh_value = 2;
					DWORD iwh_value = sizeof (wh_value);
					WriteProcessMemory (hHandle, (LPVOID) (ClientDll + 0x848798), &wh_value, iwh_value, 0);//client.dll+848798
					cout << "Wire-Wallhack =  OK! " <<  "\n" << endl;


					// r_drawrenderboxes 2 (gr�ne ESP )
					int esp_value = 1;
					DWORD iesp_value = sizeof (esp_value);
					WriteProcessMemory (hHandle, (LPVOID) (ClientDll + 0x84B9A0), &esp_value, iesp_value, 0);//client.dll+84B9A0
					cout << "ESP boxes  =  OK! " << "\n" << endl;


					//FPS
					int fps_value = 2;
					DWORD ifps_value = sizeof (fps_value);
					WriteProcessMemory (hHandle, (LPVOID) (ClientDll + 0x8ABFF8), &fps_value, ifps_value, 0);//client.dll+8ABFF8
					cout << "Draw FPS =  OK! " << "\n" << endl;



					//Glasswalls enabler
					int walls_value = 2;
					DWORD iwalls_value = sizeof (walls_value);
					WriteProcessMemory (hHandle, (LPVOID) (materialsystemDll + 0x33A400), &walls_value, iwalls_value, 0);//materialsystem.dll+33A400
					cout << "Glasswalls = Loading.. " << "\n" << endl;
					cout << "...... \n " << endl;

					Sleep (3000); // wait some seconds and then enable glasswalls

					//Glasswalls final code
					int walls2_value = 1;
					DWORD iwalls2_value = sizeof (walls2_value);
					WriteProcessMemory (hHandle, (LPVOID) (materialsystemDll + 0x33A400), &walls2_value, iwalls2_value, 0);//materialsystem.dll+33A400
					cout << "Glasswalls =  OK! " << "\n" << endl;

					//no smoke
					int smoke_value = 0;
					DWORD ismoke_value = sizeof (smoke_value);
					WriteProcessMemory (hHandle, (LPVOID) (ClientDll + 0x8A6768), &smoke_value, ismoke_value, 0);//client.dll+8A6768 // no smoke
					cout << "NO Smoke = OK! " << "\n" << endl;

					//no hands
					int hands_value = 0;
					DWORD ihands_value = sizeof (hands_value);
					WriteProcessMemory (hHandle, (LPVOID) (ClientDll + 0x8AFE98), &hands_value, ihands_value, 0);//client.dll+8AFE98//no hands
					cout << "NO Hands = OK! " << "\n" << endl;



					//////////////////////////////////////////////


					cout << "Finished loading , i will close in 10 seconds ..." << endl;
					Sleep (10000);
					exit (0);


				}
		}
	else
		{


			system ("Color 8C");
			cout << "Ohhh nooo, you have to do this while you are PLAYING...." << endl;
			cout << "\n Try again :) " << endl;

			Sleep (3000);
			exit (0);



		}

	return 0;
}
void portableSleep (int sec)
{
#   ifdef POSIX
	sleep (sec);
#   endif
#   ifdef WINDOWS
	Sleep (sec * 1500);
#   endif
}
void startup()
{
	{
		SetConsoleTitle ("SpaceHack Base by Sandaasu");
		system ("Color 80"); //8= Grey / 0= Black


		cout << "\t\t\t +++++++++++++++++++++++++++++++++" << endl;
		cout << "\t\t\t ++++++###+++++++++++++##+++##++++" << endl;
		cout << "\t\t\t +++++###++++++++++++++##+++##++++" << endl;
		cout << "\t\t\t ++++###+++++++++++++++##+++##++++" << endl;
		cout << "\t\t\t ++###+++++++++++++++++#######++++" << endl;
		cout << "\t\t\t ++###+++++++++####++++#######++++" << endl;
		cout << "\t\t\t +++++###++++++++++++++##+++##++++" << endl;
		cout << "\t\t\t +++###++++++++++++++++##+++##++++" << endl;
		cout << "\t\t\t ++###+++++++++++++++++##+++##++++" << endl;
		cout << "\t\t\t +++++++++++++++++++++++++++++++++" << endl;
		cout << "\t\t\t +++++ SpaceHack by Sandaasu  ++++" << endl;
		cout << "\t\t\t +++++++++++++++++++++++++++++++++" << "\n " << endl;


		cout << "\n \n \n  " << endl;

		cout << "-- Version 2.3 -- " << endl;

		cout << "-- Game : CS:GO -- " << "\n" << endl;

		cout << "-- Working with game version : Exe build: 13:31:32 Dec 26 2012 (5163) (730) -- " << endl;

		cout << "\n \n \n " << endl;

		cout << "Please press ENTER here after join a game..." << endl;

		cin.get();
		system ("cls");
		hack();

	}

}

int main()
{
	startup();

}



DWORD GetModuleBase (const DWORD dwProcessId, const char* szModuleName)
{
	HANDLE hSnap = CreateToolhelp32Snapshot (TH32CS_SNAPMODULE, dwProcessId);

	if (!hSnap)
		{
			return 0;
		}

	MODULEENTRY32 me;
	me.dwSize = sizeof (MODULEENTRY32);
	DWORD dwReturn = 0;

	if (Module32First (hSnap, &me))
		{
			while (Module32Next (hSnap, &me))
				{
					if (lstrcmpi (me.szModule, szModuleName) == 0)
						{
							dwReturn = (DWORD) me.modBaseAddr;
							break;
						}
				}
		}

	CloseHandle (hSnap);
	return dwReturn;
}
