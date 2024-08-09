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
	void HeavyAttack();


protected:
	virtual void BeginPlay() override;
	
	// ASRCharacter
	virtual void ResetState() override;


	// Enhanced Input
	void Input_LightAttack(const FInputActionValue& Value);
	void Input_HeavyAttack(const FInputActionValue& Value);

private:

	bool bIsLightAttackPending = false;
	bool bIsHeavyAttackPending = false;

	int32 LightAttackIndex;
	int32 HeavyAttackIndex;


	// Handling Combo Attack
	void ExecuteLightAttack(int32 AttackIndex);
	void ResetLightAttack();

	void ExecuteHeavyAttack(int32 AttackIndex);
	void ResetHeavyAttack();


	// MotionWarping



	UFUNCTION(BlueprintCallable)
	void ResolveLightAttackPending();


	UFUNCTION(BlueprintCallable)
	void ResolveHeavyAttackPending();




private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMeshComponent;

	// Enhanced Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LightAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* HeavyAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	TArray<class UAnimMontage*> LightAttackMontages;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	TArray<class UAnimMontage*> HeavyAttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	float LightAttackWarpDistance = 150.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	float HeavyAttackWarpDistance = 150.f;

};
