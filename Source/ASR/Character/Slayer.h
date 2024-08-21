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

	void FirstSkill();
	void DashLightAttack();
	void DashHeavyAttack();

	virtual float GetFirstSkillWarpDistance() const override;


protected:
	virtual void LightAttack() override;
	virtual void ResetSkills() override;
	virtual void ResetState() override;


	void Input_FirstSkill(const FInputActionValue& Value);



	virtual void ResolveLightAttackPending() override;
	virtual void SetExecutionCamera() override;



private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SlayerFirstSkillAction;

	bool bIsFirstSkillPending = false;

	void ExecuteFirstSkill();

	

	// Animations
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* FirstSkillMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DashLightAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DashHeavyAttackMontage;
	//

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float FirstSkillWarpDistance = 500.f;

};
