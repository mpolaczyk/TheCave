// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "CA.h"
#include "CAGameMode.h"
#include "CACharacter.h"

ACAGameMode::ACAGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
