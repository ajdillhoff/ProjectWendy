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

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
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

void AProjectWendyPlayerController::MoveToMouseCursor()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(Hit.ImpactPoint);
	}
}

void AProjectWendyPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void AProjectWendyPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const Pawn = GetPawn();
	if (Pawn)
	{
		UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
		float const Distance = FVector::Dist(DestLocation, Pawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if (NavSys && (Distance > 120.0f))
		{
			NavSys->SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void AProjectWendyPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void AProjectWendyPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}
