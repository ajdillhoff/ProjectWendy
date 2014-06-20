// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ProjectWendy.h"
#include "ProjectWendyPlayerController.h"
#include "EngineUserInterfaceClasses.h"

AProjectWendyPlayerController::AProjectWendyPlayerController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void AProjectWendyPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	RotateToMouseCursor();
}

void AProjectWendyPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();
}

void AProjectWendyPlayerController::RotateToMouseCursor() {
	// Local Variables
	FVector mouseLocation, mouseDirection;
	ACharacter *currentChar = this->GetCharacter();
	FRotator charRotation   = currentChar->GetActorRotation();
	FRotator targetRotation, newRotation;

	// Get the current mouse rotation
	this->DeprojectMousePositionToWorld(mouseLocation, mouseDirection);

	targetRotation = mouseDirection.Rotation();

	// Create new rotation
	newRotation = FRotator(charRotation.Pitch, targetRotation.Yaw, charRotation.Roll);

	currentChar->SetActorRotation(newRotation);

}