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
	bool CanDodge() const;

	void LightAttack();
	void HeavyAttack();
	void Dodge();


protected:
	virtual void BeginPlay() override;
	
	// ASRCharacter
	virtual void ResetState() override;


	// Enhanced Input
	void Input_LightAttack(const FInputActionValue& Value);
	void Input_HeavyAttack(const FInputActionValue& Value);
	void Input_Dodge(const FInputActionValue& Value);

private:

	// Button Pressed Amidst Other Actions
	bool bIsLightAttackPending = false;
	bool bIsHeavyAttackPending = false;
	bool bIsDodgePending = false;

	bool bIsInvulnerable = false;

	int32 LightAttackIndex;
	int32 HeavyAttackIndex;


	// Handling Combo Attack
	void ExecuteLightAttack(int32 AttackIndex);
	void ResetLightAttack();

	void ExecuteHeavyAttack(int32 AttackIndex);
	void ResetHeavyAttack();

	FORCEINLINE void ResetDodgeAttack() { bIsDodgePending = false; }


	// Resolve Pending Actions
	UFUNCTION(BlueprintCallable)
	void ResolveLightAttackPending();

	UFUNCTION(BlueprintCallable)
	void ResolveHeavyAttackPending();

	UFUNCTION(BlueprintCallable)
	void ResolveDodgePending();
	
	UFUNCTION(BlueprintCallable)
	bool IsInvulnerable() { return bIsInvulnerable; }

	UFUNCTION(BlueprintCallable)
	void SetInvulnerable(bool InInvulnerable) { bIsInvulnerable = InInvulnerable; }




private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMeshComponent;

	// Enhanced Input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LightAttackAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* HeavyAttackAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DodgeAction;

	// Animations
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	TArray<class UAnimMontage*> LightAttackMontages;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	TArray<class UAnimMontage*> HeavyAttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	float LightAttackWarpDistance = 150.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	float HeavyAttackWarpDistance = 150.f;


};
