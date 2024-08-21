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

	virtual float GetFirstSkillWarpDistance() const override;


protected:
	virtual void LightAttack() override;

	void Input_FirstSkill(const FInputActionValue& Value);



	virtual void ResolveLightAttackPending() override;



private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SlayerFirstSkillAction;

	bool bIsFirstSkillPending = false;

	void ExecuteFirstSkill();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* FirstSkillMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float FirstSkillWarpDistance = 500.f;

};
