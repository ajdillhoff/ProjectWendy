// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "ProjectWendyCharacter.generated.h"

UCLASS(Blueprintable)
class AProjectWendyCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

	/** spawn inventory, setup initial variables */
	virtual void PostInitializeComponents() OVERRIDE;

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TSubobjectPtr<UCameraComponent> TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TSubobjectPtr<class USpringArmComponent> CameraBoom;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** get weapon attach point */
	FName GetWeaponAttachPoint() const;

	/** Get the current pawn's mesh */
	USkeletalMeshComponent* GetPawnMesh() const;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) OVERRIDE;

	void OnFire();

	void MoveForward(float Value);

	void MoveRight(float Value);

	/** socket or bone name for attaching weapon mesh */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	FName WeaponAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	AProjectWendyWeapon* Weapon;

	/** pawn mesh: 1st person view */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	TSubobjectPtr<USkeletalMeshComponent> CharacterMesh;
};

