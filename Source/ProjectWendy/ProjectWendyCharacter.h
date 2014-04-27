// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "ProjectWendyCharacter.generated.h"

UCLASS(Blueprintable)
class AProjectWendyCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TSubobjectPtr<UCameraComponent> TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TSubobjectPtr<class USpringArmComponent> CameraBoom;

protected:
	void MoveForward(float Value);
	void MoveRight(float Value);

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) OVERRIDE;

};

