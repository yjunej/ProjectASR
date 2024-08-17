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
	
	bool CanAttack() const;
	bool CanDodge() const;
	bool CanAttakInAir() const;

	void LightAttack();
	void HeavyAttack();
	void DashLightAttack();
	void DashHeavyAttack();
	void Execution();
	void Dodge();
	void UseFirstSkill();

	void PlayUltAttackMontage();
	void UltEnd();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	float LightAttackWarpDistance = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	float HeavyAttackWarpDistance = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	float DashAttackWarpDistance = 500.f;


protected:
	virtual void BeginPlay() override;
	
	// ASRCharacter
	virtual void ResetState() override;
	virtual void Input_Move(const FInputActionValue& Value) override;
	virtual void Input_Execution(const FInputActionValue& Value) override;
	virtual void ResetCamera() override;

	virtual void Input_Guard(const FInputActionValue& Value) override;
	

	// Enhanced Input
	void Input_LightAttack(const FInputActionValue& Value);
	void Input_HeavyAttack(const FInputActionValue& Value);
	void Input_Dodge(const FInputActionValue& Value);
	void Input_FirstSkill(const FInputActionValue& Value);
	void Input_Ult(const FInputActionValue& Value);
	void Input_Release_Ult(const FInputActionValue& Value);





private:

	// Button Pressed Amidst Other Actions
	bool bIsLightAttackPending = false;
	bool bIsHeavyAttackPending = false;
	bool bIsFirstSkillPending = false;

	bool bIsDodgePending = false;

	int32 LightAttackIndex;
	int32 HeavyAttackIndex;


	// Handling Combo Attack
	void ExecuteLightAttack(int32 AttackIndex);
	void ExecuteHeavyAttack(int32 AttackIndex);
	void ExecuteAerialAttack();


	// Override Parent Hook 
	virtual void ResetLightAttack() override;
	virtual void ResetHeavyAttack() override;
	virtual void ResetFirstSkill() override;
	virtual void ResetDodge() override;



	// Aerial Combo
	bool AerialAttack();

	void ExecuteLightAttackInAir(int32 AttackIndex);

	// Ult
	bool bIsUltCharging = false;
	int32 UltTargetIndex = 0;
	void ResetUlt();


	// Resolve Pending Actions
	UFUNCTION(BlueprintCallable)
	void ResolveLightAttackPending();

	UFUNCTION(BlueprintCallable)
	void ResolveHeavyAttackPending();

	UFUNCTION(BlueprintCallable)
	void ResolveDodgePending();
	
	UFUNCTION(BlueprintCallable)
	bool IsInvulnerable() const { return bIsInvulnerable; }

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FirstSkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* UltAction;


	// Animations
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	TArray<class UAnimMontage*> LightAttackMontages;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	TArray<class UAnimMontage*> HeavyAttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DodgeMontage;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AerialAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AerialSmashMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DashLightAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DashHeavyAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* UltReadyMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* UltAttackMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* UltLastAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	TArray<class UAnimMontage*> LightAttackInAirMontages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ExecutionMontage;


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
