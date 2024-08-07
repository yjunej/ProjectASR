// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Ranger.generated.h"

class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

UENUM(BlueprintType)
enum class ETurningInPlace : uint8
{
	ETIP_Left UMETA(DisplayName = "Turning in place Left"),
	ETIP_Right UMETA(DisplayName = "Turning in place Right"),
	ETIP_None UMETA(DisplayName = "Not Turning"),
	
	ETIP_MAX UMETA(Hidden),
};


UENUM(BlueprintType)
enum class ERangerBattleState : uint8
{
	ERS_Normal UMETA(DisplayName = "Normal"),
	ERS_Reloading UMETA(DisplayName = "Reloading"),

	ERS_MAX UMETA(Hidden),
};


UCLASS()
class ASR_API ARanger : public ACharacter
{
	GENERATED_BODY()

public:
	ARanger();

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void Jump() override;

protected:
	virtual void BeginPlay() override;

protected:

	// Enhanced Input
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Equip(const FInputActionValue& Value);
	void ToggleCrouch(const FInputActionValue& Value);
	void Aim(const FInputActionValue& Value);
	void StopAiming(const FInputActionValue& Value);
	void Fire(const FInputActionValue& Value);
	void StopFiring(const FInputActionValue& Value);
	void Reload(const FInputActionValue& Value);
	//

	void TurnInPlace(float DeltaSeconds);


private:
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HUD, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* CharacterInfoWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Battle, meta = (AllowPrivateAccess = "true"))
	class URangerBattleComponent* BattleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Battle, meta = (AllowPrivateAccess = "true"))
	ERangerBattleState RangerBattleState = ERangerBattleState::ERS_Normal;


	// Replication

	UPROPERTY(ReplicatedUsing = OnRep_OverlappedWeapon)
	class ARangerWeapon* OverlappedWeapon;

	UFUNCTION()
	void OnRep_OverlappedWeapon(ARangerWeapon* LastWeapon);

	UFUNCTION(Server, Reliable)
	void ServerEquip(const FInputActionValue& Value);


	
	// Animation 

	UPROPERTY(EditAnywhere, Category = Animation)
	class UAnimMontage* FireMontage;

	UPROPERTY(EditAnywhere, Category = Animation)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Animation)
	UAnimMontage* ReloadMontage;



private:
	// Enhanced Input


	// [TODO] BASE CLASE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleCrouchAction;

	// [TODO] BASE CLASE

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipAction;
	



	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;



public:

	float AimOffsetYaw;
	float InterpAimOffsetYaw;
	float AimOffsetPitch;
	FRotator PrevAimRotation;
	ETurningInPlace TurningInPlace;


	// Weapon Equip
	bool IsWeaponEquipped();
	bool IsAiming();
	
	void SetOverlappedWeapon(ARangerWeapon* Weapon);
	void UpdateAimOffset(float DeltaSeconds);
	ARangerWeapon* GetEquippedWeapon() const;

	FVector GetHitPoint() const;


	// Animation
	void PlayFireMontage(bool bAiming);
	void PlayHitReactMontage();
	void PlayReloadMontage();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastHit();



	// Camera
	float CameraThreshold = 200.f;
	void HideCharacterClosedCamera();

	// Aim

	float DefaultSpringArmLength = 230.f;
	FVector DefaultSpringArmSocketOffest = FVector(0.f, 40.f, 70.f);
	FVector DefaultSpringArmLocation = FVector(0.f, 0.f, 115.f);



	// [TODO] BASE CLASS
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	// [TODO] BASE CLASS

	FORCEINLINE ERangerBattleState GetRangerBattleState() const { return RangerBattleState; }
	FORCEINLINE void SetRangerBattleState(ERangerBattleState NewRangerBattleState) { RangerBattleState = NewRangerBattleState; }


};
