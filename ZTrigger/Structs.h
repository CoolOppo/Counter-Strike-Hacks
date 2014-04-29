#pragma once

#include "Offsets.h"

#define ReadMem( pOffset, pDest, pSize )			ReadProcessMemory( gProcess.__HandleProcess, (PBYTE*)pOffset, pDest, sizeof( pSize ), 0 );
#define WriteMem( pOffset, pDest, pSize )			WriteProcessMemory( gProcess.__HandleProcess, (PBYTE*)pOffset, pDest, sizeof( pSize ), 0 );

struct MyPlayer_t
{
	DWORD CLocalPlayer;
	float Position[3];
	int Team;
	int Health;

	void ReadInformation()
	{
		ReadMem ( (gProcess.__dwordClient + dwPlayerBase), &CLocalPlayer, DWORD);
		ReadMem ( (CLocalPlayer + dwTeam), &Team, int);
		ReadMem ( (CLocalPlayer + dwHealth), &Health, int);
		ReadMem ( (CLocalPlayer + dwPosition), &Position, float[3]);
	}

	void SetView (float fAngle[])
	{
		DWORD CViewPtr;
		ReadMem ( (gProcess.__dwordEngine + dwEnginePtr), &CViewPtr, DWORD);
		WriteMem ( (CViewPtr + dwSetAngle), fAngle, 12);
	}

} MyPlayer;

struct PlayerList_t
{
	DWORD CBaseEntity;
	float Position[3];
	float AimAngle[3];
	int Team;
	int Health;

	void ReadInformation (int nIndex)
	{
		ReadMem ( (gProcess.__dwordClient + dwEntityBase + (nIndex * dwEntitySize)), &CBaseEntity, DWORD);
		ReadMem ( (CBaseEntity + dwTeam), &Team, int);
		ReadMem ( (CBaseEntity + dwHealth), &Health, int);
		ReadMem ( (CBaseEntity + dwPosition), &Position, float[3]);
	}

} PlayerList[32];

void CalcAngle (float* src, float* dst, float* angles)
{
	double delta[3] = { (src[0] - dst[0]), (src[1] - dst[1]), (src[2] - dst[2]) };
	double hyp = sqrt (delta[0] * delta[0] + delta[1] * delta[1]);
	angles[0] = (float) (asinf (delta[2] / hyp) * 57.295779513082f);
	angles[1] = (float) (atanf (delta[1] / delta[0]) * 57.295779513082f);
	angles[2] = 0.0f;

	if (delta[0] >= 0.0)
		{
			angles[1] += 180.0f;
		}
}
