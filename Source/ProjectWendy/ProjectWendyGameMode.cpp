// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ProjectWendy.h"
#include "ProjectWendyGameMode.h"
#include "ProjectWendyPlayerController.h"

AProjectWendyGameMode::AProjectWendyGameMode(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
	// use our custom PlayerController class
	PlayerControllerClass = AProjectWendyPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FObjectFinder<UClass> PlayerPawnBPClass(TEXT("Class'/Game/Blueprints/MyCharacter.MyCharacter_C'"));
	if (PlayerPawnBPClass.Object != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Object;

	}
}