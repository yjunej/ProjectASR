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
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void FirstSkill(); // Key E 
	void SecondSkill(); // Key R
	void DashAttack();

	virtual float GetFirstSkillWarpDistance() const override;


protected:

	// Hook
	virtual void NormalAttack() override;
	virtual void ResetSkills() override;
	virtual void ResolveHeavyAttackPending() override;


	virtual void ResetState() override;


	void Input_FirstSkill(const FInputActionValue& Value);
	void Input_SecondSkill(const FInputActionValue& Value);


	virtual void ResolveLightAttackPending() override;
	virtual void SetExecutionCamera() override;



private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMeshComponent;

	// Enhanced Inpyut
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SlayerFirstSkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SlayerSecondSkillAction;


	bool bIsFirstSkillPending = false;
	bool bIsSecondSkillPending = false;

	void ExecuteFirstSkill();
	void ExecuteSecondSkill();
	

	// Animations
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* FirstSkillMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* SecondSkillMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DashAttackMontage;
	//

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float FirstSkillWarpDistance = 500.f;

};
