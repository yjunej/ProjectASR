// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ASRCharacter.h"
#include "Gunner.generated.h"

/**
 *
 */
UENUM(BlueprintType)
enum class EGunnerAnimState : uint8
{
	EGA_Normal UMETA(DisplayName = "Normal"),
	EGA_Reloading UMETA(DisplayName = "Reloading"),

	EGA_MAX UMETA(Hidden),
};

UCLASS()
class ASR_API AGunner : public AASRCharacter
{
	GENERATED_BODY()

public:
	AGunner();

	virtual void Tick(float DeltaSeconds) override;
	virtual void PostInitializeComponents() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void FirstSkill(); // Key E 
	void SecondSkill(); // Key R
	void DashAttack();

	virtual float GetFirstSkillWarpDistance() const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMeshComponent;


	// Anim Instance 
	bool IsAiming();
	bool bIsAiming = false;
	float AimOffsetYaw;
	float AimOffsetPitch;
	EGunnerAnimState GunnerAnimState;
	FVector HitPoint;
	void TraceCrosshairs(FHitResult& HitResult);
	void SetHUDCrosshair(float DeltaSeconds);
	void InterpFOV(float DeltaSeconds);
	UPROPERTY(EditAnywhere, Category = Zoom)
	float CrouchArmHeightOffset = -50.f;
	UPROPERTY(EditAnywhere, Category = Zoom)
	float ZoomArmLength = 80.f;
	UPROPERTY(EditAnywhere, Category = Zoom)
	FVector ZoomArmSocketOffset = { 0.f, 60.f, 40.f };
	UPROPERTY(EditAnywhere, Category = Zoom)
	float ZoomFOV = 30.f;
	UPROPERTY(EditAnywhere, Category = Zoom)
	float ZoomSpeed = 20.f;
	float DefaultFOV;
	float FOV;
	UPROPERTY(EditAnywhere)
	float ArmedMaxWalkSpeed = 600.f;
	UPROPERTY(EditAnywhere)
	float ArmedMaxWalkSpeedAiming = 350.f;
	class AASRPlayerController* GunnerPlayerController;
	float SpreadRate;
	float CrosshairInAirCoef;
	float CrosshairCrouchCoef;
	float CrosshairAimCoef;
	FVector GetHitPoint() const;
	float DefaultSpringArmLength = 230.f;
	FVector DefaultSpringArmSocketOffest = FVector(0.f, 40.f, 70.f);
	FVector DefaultSpringArmLocation = FVector(0.f, 0.f, 115.f);



protected:
	virtual void BeginPlay() override;

	// Hook
	virtual void NormalAttack() override;
	virtual void ResetSkills() override;
	virtual void ResolveHeavyAttackPending() override;
	virtual void OnAttackEnemy() override;
	//

	virtual void ResetState() override;

	// Enhanced Input
	void Input_FirstSkill(const FInputActionValue& Value);
	void Input_SecondSkill(const FInputActionValue& Value);
	void Input_Ult(const FInputActionValue& Value);

	// Gunner Special Input
	void Input_Fire(const FInputActionValue& Value);
	void Input_StopFiring(const FInputActionValue& Value);
	void Input_Aim(const FInputActionValue& Value);
	void Input_StopAiming(const FInputActionValue& Value);
	void Input_Reload(const FInputActionValue& Value);

	virtual void ResolveLightAttackPending() override;




private:


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

	// Gunner Specific
	void Fire(bool bFire);
	bool bStartFire = false;
	void FireWithTimer();
	bool bCanFire = true;
	bool CanFire();
	UPROPERTY(EditAnywhere, Category = Weapon)
	int32 Ammo;
	void PlayFireMontage(bool bAiming);
	FTimerHandle FireTimer;
	void StartFireTimer();
	void EndFireTimer();
	void Reload();
	UFUNCTION(BlueprintCallable)
	void ReloadFinished();
	void PlayReloadMontage();
	void WeaponFire(const FVector& HitTargetPoint);
	FTimerHandle CrosshairTimerHandle;
	void OnHitEnemy();
	void ResetCrosshairColor();
	UPROPERTY(EditAnywhere, Category = Weapon)
	TSubclassOf<class ABulletCase> CaseClass;

	UPROPERTY(EditAnywhere, Category = Weapon)
	TSubclassOf<class AProjectile> ProjectileClass;

	// Animations
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* FirstSkillMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* SecondSkillMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* FireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category = Animation)
	class UAnimationAsset* WeaponFireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;

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
