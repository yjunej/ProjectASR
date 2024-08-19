// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ASRCharacter.h"
#include "Slayer.generated.h"

/**
 * 
 */
UCLASS()
class ASR_API ASlayer : public AASRCharacter
{
	GENERATED_BODY()

public:
	ASlayer();

	virtual void PostInitializeComponents() override;

protected:
	virtual void LightAttack() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMeshComponent;
};
