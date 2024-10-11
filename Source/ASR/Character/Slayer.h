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
	virtual void BeginPlay() override;

	// Hook
	virtual void NormalAttack() override;
	//virtual void ResetSkills() override;
	virtual void ResolveHeavyAttackPending() override;
	//

	virtual void ResetState() override;

	// Enhanced Input
	void Input_FirstSkill(const FInputActionValue& Value);
	void Input_SecondSkill(const FInputActionValue& Value);
	void Input_Ult(const FInputActionValue& Value);


	virtual void ResolveLightAttackPending() override;
	


private:
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<class AMeleeWeapon> MeleeWeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	AMeleeWeapon* MeleeWeapon;

	// Enhanced Inpyut
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SlayerFirstSkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SlayerSecondSkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SlayerUltAction;

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
	UAnimMontage* UltMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DashAttackMontage;
	//

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* UltCameraChildComponent;

	UFUNCTION(BlueprintCallable)
	void SetUltCamera();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float FirstSkillWarpDistance = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	FTransform UltCameraTransform = FTransform(
		FRotator(-28.f, 23.f, -11.f),
		FVector(-905.f, -490.f, 678.f),
		FVector(1.f, 1.f, 1.f));
};
