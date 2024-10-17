// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ASR/Enums/ASRDamageType.h"
#include "ASR/Enums/CombatState.h"
#include "CombatInterface.generated.h"

USTRUCT(BlueprintType)
struct FHitData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FHitData()
	: Damage(0.f)
	, DamageType(EASRDamageType::EDT_Default)
	, HitSound(nullptr)
	, HitEffect(nullptr)
	, HitEffectRotation(FRotator::ZeroRotator)
	, HitEffectScale(FVector::OneVector)
	, HitParticleEffect(nullptr)
	, bIsLethalAttack(false)
	, bIsFatalAttack(false)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EASRDamageType DamageType;

	// TODO - Use Soft Refernce After check profile

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* HitSound;
	//TSoftObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UNiagaraSystem* HitEffect;
	//TSoftObjectPtr<class UNiagaraSystem> HitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator HitEffectRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector HitEffectScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* HitParticleEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLethalAttack = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsFatalAttack = false;
	//TSoftObjectPtr<UParticleSystem> HitParticleEffect;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ASR_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Damage System
	virtual bool GetHit(const FHitResult& HitResult, AActor* Attacker, const FHitData& HitData) = 0;

	// State Management
	UFUNCTION(BlueprintCallable)
	virtual ECombatState GetCombatState() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual EHitReactionState GetHitReactionState() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual void SetHitReactionState(EHitReactionState NewState) = 0;

	UFUNCTION(BlueprintCallable)
	virtual bool IsDead() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual bool IsInvulnerable() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual void SetInvulnerable(bool bNewInvulnerable) = 0;


	UFUNCTION(BlueprintCallable)
	virtual UDataTable* GetAttackDataTable() const = 0;

	// Attack Trace
	UFUNCTION(BlueprintCallable)
	virtual void SphereTrace(float TraceDistance, float TraceRadius, const FHitData& HitData, ECollisionChannel CollisionChannel, bool bDrawDebugTrace) = 0;


	// Attack Token System
	UFUNCTION(BlueprintCallable)
	virtual bool ReserveAttackTokens(int32 Amount) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void ReturnAttackTokens(int32 Amount) = 0;


};
