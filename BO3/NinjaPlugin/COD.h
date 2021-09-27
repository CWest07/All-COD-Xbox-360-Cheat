#pragma once

#include "stdafx.h"
#include "Utilities.h"
#include "Config.h"




class CODClient {
public:
	int index;
	WORD entityFlags;
	WORD entityType;

	char* targetBone;
	DWORD DObj;
	Vector3 headPos;
	Vector3 aimTargetPos;

	DWORD* cEnt;

	DWORD currentWeapon;

	float distance;

	BOOL visible;
	BOOL isWallBangable;
	BOOL isAlive;
	BOOL isEnemy;
	BOOL isEntPlayer;
	BOOL isEntDog;
	BOOL isOnScreen;
	WORD team;
	BOOL usingShield;
	BOOL validTarget;
	BOOL isUnknownEntity;
	char *name;
	char nameDefault[100];


	DWORD visibleBoneIndex;

	CODClient()
	{
		memset(this, 0, sizeof(CODClient));
		name = nameDefault;
		validTarget = FALSE;
	}

	void reset()
	{
		isEnemy = FALSE;
		isOnScreen = FALSE;
		isAlive = FALSE;
		validTarget = FALSE;
		visible = FALSE;
		usingShield = FALSE;
		isWallBangable = FALSE;
		isEntDog = FALSE;
		isUnknownEntity = FALSE;
		isEntPlayer = FALSE;

		entityType = 0;
		entityFlags = 0;
	}
};



