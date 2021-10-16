// Copyright Epic Games, Inc. All Rights Reserved.

#include "MP2GameMode.h"
#include "MP2HUD.h"
#include "MP2Character.h"
#include "UObject/ConstructorHelpers.h"

AMP2GameMode::AMP2GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AMP2HUD::StaticClass();
}
