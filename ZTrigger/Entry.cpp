#include <Windows.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>
using namespace std;

#include "CProcess.h"

#define Msg( x )		cout << x << endl;

CProcess gProcess;

#include "Structs.h"

VOID GetRandomAlphanum(CHAR* s, int len)
{
	static const char ranAlph[] = "0123456789"
																"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
																"abcdefghijklmnopqrstuvwxyz";

	for(int i = 0; i < len; i++)
	{
		s[i] = ranAlph[rand() % (sizeof(ranAlph) - 1)];
	}

	s[len] = 0;
}

VOID AimThread()
{
	while(true)
	{
		MyPlayer.ReadInformation();

		for(int i = 0; i < 32; i++)
		{
			if((GetAsyncKeyState(0x01)))
			{
				PlayerList[i].ReadInformation(i);

				if(MyPlayer.Team == PlayerList[i].Team)
				{
					continue;
				}

				if(PlayerList[i].Health < 2)
				{
					continue;
				}

				CalcAngle(MyPlayer.Position, PlayerList[i].Position, PlayerList[i].AimAngle);
				DWORD CViewPtr;
				ReadMem((gProcess.__dwordEngine + dwEnginePtr), &CViewPtr, DWORD);
				WriteMem((CViewPtr + dwSetAngle), PlayerList[i].AimAngle, float[3]);
			}
		}

		Sleep(15);
	}
}

int main()
{
	char title[32];
	GetRandomAlphanum(title, 32);
	SetConsoleTitle(title);
	Msg("Searching CS:GO...");
	gProcess.RunProcess();
	Msg("Found CS:GO!");
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE) AimThread, 0, 0, 0);
	Msg("Hack initiated");
	getchar();
	return 0;
}
