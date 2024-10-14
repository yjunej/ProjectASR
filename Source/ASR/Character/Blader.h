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

	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	bool CanAttakInAir() const;

	//void HeavyAttack();
	void DashAttack();
	void DashHeavyAttack();
	void FirstSkill();

	void PlayUltAttackMontage();
	void UltEnd();



	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	//float HeavyAttackWarpDistance = 150.f;




protected:
	virtual void BeginPlay() override;
	
	// ASRCharacter
	//virtual void Input_Move(const FInputActionValue& Value) override;
	virtual void Input_Execution(const FInputActionValue& Value) override;
	virtual	void Input_Dodge(const FInputActionValue& Value) override;
	virtual void Input_Guard(const FInputActionValue& Value) override;
	virtual void Input_NormalAttack(const FInputActionValue& Value) override;

	virtual void ResetState() override;

	virtual bool CanAttack() const override;
	//virtual void ResolveLightAttackPending() override;


	// Enhanced Input
	//void Input_HeavyAttack(const FInputActionValue& Value);
	//void Input_FirstSkill(const FInputActionValue& Value);
	void Input_Ult(const FInputActionValue& Value);
	void Input_Release_Ult(const FInputActionValue& Value);
	void Input_SuperDodge(const FInputActionValue& Value);


	// Override Parent Hook 
	// TODO - Categorize Reset Func (Index Based or State Based)
	virtual void NormalAttack(int32 AttackIndex) override;
	//virtual void ResetSkills() override;
	//virtual void ResolveHeavyAttackPending(int32 AttackIndex) override;


private:

	// Button Pressed Amidst Other Actions
	bool bIsHeavyAttackPending = false;
	bool bIsFirstSkillPending = false;

	int32 HeavyAttackIndex;


	// Handling Combo Attack
	//void ExecuteHeavyAttack(int32 AttackIndex);
	void ExecuteAerialAttack();


	void ResetHeavyAttack();

	// Aerial Combo
	bool AerialAttack();
	void ExecuteNormalAttackInAir(int32 AttackIndex);

	// Ult
	bool bIsUltCharging = false;
	int32 UltTargetIndex = 0;
	void ResetUlt();




	
	UFUNCTION(BlueprintCallable)
	bool IsInvulnerable() const { return bIsInvulnerable; }


private:
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<class AMeleeWeapon> MeleeWeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	AMeleeWeapon* MeleeWeapon;

	// Enhanced Input
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	//UInputAction* HeavyAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* BladerFirstSkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* UltAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SuperDodgeAction;


	// Animations
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	//TArray<class UAnimMontage*> HeavyAttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AerialAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AerialSmashMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DashAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DashHeavyAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* UltReadyMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* UltAttackMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* UltLastAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* SuperDodgeMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	TArray<class UAnimMontage*> NormalAttackInAirMontages;



	//

	// Aerial Attack TimeLine
	UPROPERTY(VisibleAnywhere, Category = Timeline)
	class UTimelineComponent* TimelineComponent;

	UFUNCTION()
	void HandleTimelineUpdate(float Value);

	UPROPERTY(EditAnywhere, Category = Timeline)
	UCurveFloat* FloatCurve;

	void InitializeTimeline();
	void StopTimeline();

	UFUNCTION(BlueprintCallable)
	void Levitate();
	bool bIsLevitating = false;
	bool bCanAttackInAir = true;

	FVector LevitateLocation;
	float LevitateHeight = 500.f;
	//

	//Ult
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Skill, meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> UltTargets;
	FTransform UltStartTransform;
	UFUNCTION(BlueprintCallable)
	void ApplyUltDamage();



public:
	FORCEINLINE TArray<AActor*> GetUltTargets() const { return UltTargets; }
	FORCEINLINE int32 GetUltTargetIndex() const { return UltTargetIndex; }
	FORCEINLINE void SetUltTargetIndex(int32 Index) { UltTargetIndex = Index; }

};
