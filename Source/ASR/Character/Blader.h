// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ASRCharacter.h"
#include "Blader.generated.h"

/**
 * 
 */
UCLASS()
class ASR_API ABlader : public AASRCharacter
{
	GENERATED_BODY()

public:
	ABlader();

	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;
};
