// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "ProjectWendyPlayerController.generated.h"

UCLASS()
class AProjectWendyPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()

protected:

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) OVERRIDE;
	virtual void SetupInputComponent() OVERRIDE;
	// End PlayerController interface

	/* Rotate character to face the cursor */
	void RotateToMouseCursor();
};


