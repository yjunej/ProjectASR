// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ASR/Enums/ASRDamageType.h"
#include "ASR/Enums/CombatState.h"
#include "HitInterface.generated.h"

USTRUCT(BlueprintType)
struct FHitData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EASRDamageType DamageType = EASRDamageType::EDT_Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* HitSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UNiagaraSystem* HitEffect = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* HitParticleEffect = nullptr;

};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UHitInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ASR_API IHitInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//
	virtual void GetHit(const FHitResult& HitResult, AActor* Attacker, const FHitData& HitData) = 0;

	UFUNCTION(BlueprintCallable)
	virtual ECombatState GetCombatState() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual bool IsDead() const = 0;

};
