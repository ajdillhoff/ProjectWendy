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

	/*********************************
	*           Inventory            *
	*********************************/

	/* Add a weapon to player inventory */
	void AddWeapon(class AProjectWendyWeapon* Weapon);

	/* Equip a weapon from inventory */
	void EquipWeapon(class AProjectWendyWeapon* Weapon);

	/* Set the current weapon */
	void SetCurrentWeapon(class AProjectWendyWeapon* NewWeapon);

	/*********************************
	*          Weapon Usage          *
	*********************************/

	/* starts weapon fire */
	void StartWeaponFire();

	/* stops weapon fire */

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) OVERRIDE;

	void MoveForward(float Value);

	void MoveRight(float Value);

	/** socket or bone name for attaching weapon mesh */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	FName WeaponAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TArray<TSubclassOf<class AProjectWendyWeapon> > DefaultWeapons;

	UPROPERTY(Transient)
	TArray<class AProjectWendyWeapon*> Inventory;

	UPROPERTY(Transient)
	class AProjectWendyWeapon* CurrentWeapon;

	/** pawn mesh: 1st person view */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	TSubobjectPtr<USkeletalMeshComponent> CharacterMesh;

	/******************
	*    Inventory    *
	******************/

	/* Set up the default inventory (weapons) */
	void SpawnDefaultInventory();

	/***********************
	*     Weapon Usage     *
	***********************/

	/* starts weapon fire */
	void OnFire();
};

