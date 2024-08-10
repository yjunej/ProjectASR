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
	virtual void GetHit(const FHitResult& HitResult, float Damage, EASRDamageType DamageType) override;


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

public:
	void SetCharacterState(EASRCharacterState InCharacterState);



	


};
