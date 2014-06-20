// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ProjectWendy.h"
#include "ProjectWendyCharacter.h"
#include "Projectile.h"
#include "ProjectWendyWeapon.h"

AProjectWendyCharacter::AProjectWendyCharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	CharacterMesh = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("HeroTPP"));
	CharacterMesh->SetCollisionObjectType(ECC_Pawn);
	CharacterMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CharacterMesh->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	CharacterMesh->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	CharacterMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

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

void AProjectWendyCharacter::PostInitializeComponents() {
	Super::PostInitializeComponents();

	// Spawn the weapon
	SpawnDefaultInventory();
}

void AProjectWendyCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent) {
	// Set up gameplay key bindings
	check(InputComponent);

	// Weapon Actions
	InputComponent->BindAction("Fire", IE_Pressed, this, &AProjectWendyCharacter::OnFire);

	// Movement
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

FName AProjectWendyCharacter::GetWeaponAttachPoint() const {
	return WeaponAttachPoint;
}

USkeletalMeshComponent* AProjectWendyCharacter::GetPawnMesh() const {
	return CharacterMesh;
}

/*********************************
*          Weapon Usage          *
*********************************/

void AProjectWendyCharacter::OnFire()
{
	StartWeaponFire();
	//// try and play the sound if specified
	//if (FireSound != NULL)
	//{
	//	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	//}

	//// try and play a firing animation if specified
	//if (FireAnimation != NULL)
	//{
	//	// Get the animation object for the arms mesh
	//	UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
	//	if (AnimInstance != NULL)
	//	{
	//		AnimInstance->Montage_Play(FireAnimation, 1.f);
	//	}
	//}

}

void AProjectWendyCharacter::StartWeaponFire() {
	if (CurrentWeapon) {
		CurrentWeapon->StartFire();
	}
}

/*****************************
*          Inventory         *
*****************************/

void AProjectWendyCharacter::SpawnDefaultInventory() {
	if (DefaultWeapons[0]) {
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.bNoCollisionFail = true;
		AProjectWendyWeapon *NewWeapon = GetWorld()->SpawnActor<AProjectWendyWeapon>(DefaultWeapons[0], SpawnInfo);
		// Add the weapon to the player's inventory
		AddWeapon(NewWeapon);
	}

	// equip first weapon in inventory
	if (Inventory.Num() > 0)
	{
		EquipWeapon(Inventory[0]);
	}
}

void AProjectWendyCharacter::AddWeapon(AProjectWendyWeapon* Weapon) {
	if (Weapon) {
		// Set the current character as the weapon owner
		Weapon->OnEnterInventory(this);
		Inventory.AddUnique(Weapon);
	}
}

void AProjectWendyCharacter::EquipWeapon(AProjectWendyWeapon* Weapon) {
	if (Weapon) {
		SetCurrentWeapon(Weapon);
	}
}

void AProjectWendyCharacter::SetCurrentWeapon(class AProjectWendyWeapon* NewWeapon) {
	CurrentWeapon = NewWeapon;

	if (NewWeapon) {
		NewWeapon->SetOwningPawn(this);
		NewWeapon->OnEquip();
	}
}