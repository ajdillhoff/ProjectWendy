// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ProjectWendy.h"
#include "ProjectWendyCharacter.h"

AProjectWendyCharacter::AProjectWendyCharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// Set size for player capsule
	CapsuleComponent->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	CharacterMovement->bOrientRotationToMovement = true; // Rotate character to moving direction
	CharacterMovement->RotationRate = FRotator(0.f, 640.f, 0.f);
	CharacterMovement->bConstrainToPlane = true;
	CharacterMovement->bSnapToPlaneAtStart = true;
	CharacterMovement->JumpZVelocity = 600.0f;
	CharacterMovement->AirControl = 0.2f;

	// Create a camera boom...
	CameraBoom = PCIP.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->RelativeRotation = FRotator(-90.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("TopDownCamera"));
	TopDownCameraComponent->AttachTo(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUseControllerViewRotation = false; // Camera does not rotate relative to arm

}

void AProjectWendyCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent) {
	// Set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAxis("MoveForward", this, &AProjectWendyCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AProjectWendyCharacter::MoveRight);
}

void AProjectWendyCharacter::MoveForward(float Value) {

	if ((Controller != NULL) && (Value != 0.0f)) {
		const FRotator rotation = this->GetActorRotation();
		const FRotator targetRotation = FRotator(rotation.Pitch, 0.0f, rotation.Roll);
		const FVector direction = FRotationMatrix(targetRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(direction, Value);
	}
}

void AProjectWendyCharacter::MoveRight(float Value) {
	if ((Controller != NULL) && (Value != 0.0f)) {
		const FRotator rotation = this->GetActorRotation();
		const FRotator targetRotation = FRotator(rotation.Pitch, 0.0f, rotation.Roll);
		const FVector direction = FRotationMatrix(targetRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(direction, Value);
	}
}