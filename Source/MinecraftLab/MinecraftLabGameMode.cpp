// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinecraftLabGameMode.h"
#include "MinecraftLabCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMinecraftLabGameMode::AMinecraftLabGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
