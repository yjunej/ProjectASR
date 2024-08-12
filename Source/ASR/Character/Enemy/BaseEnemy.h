// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ASR/Interfaces/HitInterface.h"
#include "ASR/Enums/ASRDamageType.h"
#include "ASR/Character/ASRCharacter.h" // Refactoring After Complete Basic System

#include "BaseEnemy.generated.h"

UCLASS()
class ASR_API ABaseEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseEnemy();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// HitInterface
	virtual void GetHit(const FHitResult& HitResult, AActor* Attacker, float Damage, EASRDamageType DamageType) override;


protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = State)
	EASRCharacterState CharacterState;

	UPROPERTY(EditDefaultsOnly, Category = Damage)
	TMap<EASRDamageType, FDamageTypeMapping> DamageTypeMappings;

	UFUNCTION(BlueprintCallable)
	virtual void ResetState();

	virtual void HandleDeath();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, meta = (AllowPrivateAccess = "true"))
	float Health;

	//UFUNCTION(BlueprintCallable) 
	UAnimMontage* GetHitReactionMontage(EASRDamageType DamageType);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathMontage;

	// TimeLine
	UPROPERTY(VisibleAnywhere, Category = "Timeline")
	class UTimelineComponent* TimelineComponent;

	UFUNCTION()
	void HandleTimelineUpdate(float Value);

	UFUNCTION()
	void HandleTimelineFinished();

	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* KnockbackCurve;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* LevitateCurve;

	void InitializeTimeline();
	void StopTimeline();
	float KnockbackDistance = 50.f;

	FVector StartLocation;
	void ApplyKnockback();
	void Levitate();
	float LevitateHeight = 500.f;
	bool bIsLevitating = false;
	bool bIsSmashing = false;



public:
	FORCEINLINE EASRCharacterState GetCharacterState() const { return CharacterState; };
	void SetCharacterState(EASRCharacterState InCharacterState);

	

	


};
