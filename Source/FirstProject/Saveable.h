// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UObject/Interface.h"
#include "Saveable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USaveable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FIRSTPROJECT_API ISaveable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual TArray<uint8> CaptureState() = 0;
	virtual void RestoreState(TArray<uint8> State, bool bIsLevelChanging) = 0;
};
