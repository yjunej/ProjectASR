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
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	

	bool CanAttack() const;
	void LightAttack();



protected:
	virtual void BeginPlay() override;
	
	// ASRCharacter
	virtual void ResetState() override;


	// Enhanced Input
	void Input_LightAttack(const FInputActionValue& Value);
	void Input_HeavyAttack(const FInputActionValue& Value);

private:

	bool bIsLightAttackPending = false;
	int32 LightAttackIndex;

	// Handling Combo Attack
	void ExecuteLightAttack(int32 AttackIndex);
	void ResetLightAttack();


	UFUNCTION(BlueprintCallable)
	void ResolveLightAttackPending();




private:

	// Enhanced Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LightAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* HeavyAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	TArray<class UAnimMontage*> LightAttackMontages;
	
	


};
